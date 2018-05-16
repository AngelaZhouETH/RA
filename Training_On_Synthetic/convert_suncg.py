import numpy as np
import argparse
import os
import sys
import glob
import skimage.io


def mkdir_if_not_exist(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)


def get_list_dir(scene_path):
    working_dir = os.getcwd()

    os.chdir(scene_path)
    file_list = [] 

    for file in glob.glob("*/"):
        file_list.append(file)

    os.chdir(working_dir)

    return file_list


def get_list_files(scene_path, ext):
    working_dir = os.getcwd()

    os.chdir(scene_path)
    file_list = [] 

    for file in glob.glob("*{}".format(ext)):
        file_list.append(file)

    file_list = sorted(file_list, key=lambda name: int(name[:6]))

    os.chdir(working_dir)

    return file_list


def read_dat_groudtruth(path):
    with open(path, "rb") as fid:
        version = np.fromfile(fid, count=1, dtype=np.uint8)
        assert version == 1

        is_big_endian = np.fromfile(fid, count=1, dtype=np.uint8)
        assert (is_big_endian == 1 and sys.byteorder == "big") or \
               (is_big_endian == 0 and sys.byteorder == "little")

        uint_size = np.fromfile(fid, count=1, dtype=np.uint8)
        assert uint_size == 4

        elem_size = np.fromfile(fid, count=1, dtype=np.uint32)
        if elem_size == 4:
            dtype = np.int32
        else:
            raise ValueError("Unsupported data type of size {}".format(elem_size))

        num_labels = np.fromfile(fid, count=1, dtype=np.uint32)[0]
        width = np.fromfile(fid, count=1, dtype=np.uint32)[0]
        height = np.fromfile(fid, count=1, dtype=np.uint32)[0]
        depth = np.fromfile(fid, count=1, dtype=np.uint32)[0]

        num_elems = width * height * depth * num_labels
        assert num_elems > 0

        grid = np.fromfile(fid, count=num_elems, dtype=dtype)
        grid =  grid.reshape(depth, height, width, num_labels)

        grid = grid.transpose(2, 1, 0, 3)

        return grid


def read_groundtruth(room_path, numclasses):

    # Read the dat file
    groundtruth = read_dat_groudtruth(os.path.join(room_path, "GroundTruth.dat"))

    # Change labeling so that free space is the last
    groundtruth = np.squeeze(groundtruth)
    groundtruth -= 1
    groundtruth[groundtruth < 0] = numclasses-1

    # Change to one hot encoding (ground truth prob)
    xres, yres, zres = np.shape(groundtruth)
    groundtruth = np.reshape(groundtruth, groundtruth.size)
    groundtruth_onehot = np.zeros((groundtruth.size, numclasses), dtype=np.float32)
    groundtruth_onehot[np.arange(groundtruth.size), groundtruth] = 1.0
    groundtruth_onehot = np.reshape(groundtruth_onehot, [xres, yres, zres, numclasses])

    return groundtruth_onehot


def read_projections(scene_path, width, height):

    cam_file = os.path.join(scene_path, "cameras.txt")

    # Initialize list of projection, rotation, position and intrinsics
    projections = []
    R = []
    C = []
    K = []

    with open(cam_file, "r") as fid:
        for line in fid:
            elems = list(map(lambda x: float(x.strip()), line.split(",")))
            elems = np.array(elems)

            # Create rigth handed orthonormal basis
            cam_center =  elems[0:3] # camera position
            towards    = -elems[3:6] # pointing direction
            updir      = -elems[6:9] # up direction

            towards /= np.linalg.norm(towards)

            right = np.cross(towards, updir)
            right /= np.linalg.norm(right)

            updir = np.cross(right, towards)

            # Create camera rotation
            cam_rot = np.eye(3)
            cam_rot[0,:] = right
            cam_rot[1,:] = updir
            cam_rot[2,:] = -towards

            # Create camera translation: T = -RC
            cam_trans = -np.matmul(cam_rot, np.expand_dims(cam_center,axis=-1))

            # Camera extrinsics P = [R|T]
            cam_pose = np.concatenate([cam_rot , cam_trans], axis=-1)

            # Camera intrinsic
            fov = elems[9:11]
            focal_length_x = width   / (2 * np.tan(fov[0]))
            focal_length_y = height  / (2 * np.tan(fov[1]))

            cam_intrinsics = np.eye(3)
            cam_intrinsics[0,0] = focal_length_x
            cam_intrinsics[1,1] = focal_length_y
            cam_intrinsics[0,2] = width/2
            cam_intrinsics[1,2] = height/2

            # Projection matrix
            proj = np.dot(cam_intrinsics, cam_pose)  # proj = K[R|T]

            # Output
            projections.append(proj.astype(np.float32))
            R.append(cam_rot)
            C.append(cam_center)
            K.append(cam_intrinsics)

    return projections, K, R, C


def read_suncg_img(scene_path, image_name, dtype=np.float32):
    image_path = os.path.join(scene_path, image_name)
    suncg_img = skimage.io.imread(image_path)
    suncg_img = suncg_img.astype(dtype)
    return suncg_img


def backproj_depth(depth_map, K, R, C):
    # Back project
    points3D = []
    dm_shape = np.shape(depth_map)

    for y in range(dm_shape[0]):
        for x in range(dm_shape[1]):
            point = np.zeros(4)
            depth = depth_map[y,x]
            point[0] = (x - K[0,2])*depth/K[0,0]
            point[1] = (y - K[1,2])*depth/K[1,1]
            point[2] = depth
            point[3] = 1.0

            cam2glob = np.eye(4)
            cam2glob[:3, :3] = R.transpose()
            cam2glob[:3, 3] = C

            point = np.dot(cam2glob, point)
            point /= point[3]

            points3D.append(point[:3])

    return points3D


def write_ply(path, points, color):
    with open(path, "w") as fid:
        fid.write("ply\n")
        fid.write("format ascii 1.0\n")
        fid.write("element vertex {}\n".format(points.shape[0]))
        fid.write("property float x\n")
        fid.write("property float y\n")
        fid.write("property float z\n")
        fid.write("property uchar diffuse_red\n")
        fid.write("property uchar diffuse_green\n")
        fid.write("property uchar diffuse_blue\n")
        fid.write("end_header\n")
        for i in range(points.shape[0]):
            fid.write("{} {} {} {} {} {}\n".format(points[i, 0], points[i, 1],
                                                   points[i, 2], *color))


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_path", type=str, default="/Users/zhoumoyuan/Documents/sem4/RA/Data_Test")
    parser.add_argument("--depth_ext", type=str, default="_kinect.png")
    parser.add_argument("--category_ext", type=str, default="_category.png")
    parser.add_argument("--width", type=int, default=640)
    parser.add_argument("--height", type=int, default=480)
    parser.add_argument("--visualize_proj", action='store_true')
    return parser.parse_args()


def main():
    args = parse_args()
    
    data_path  = args.data_path

    # Get list of scenes
    scene_list = get_list_dir(data_path)

    # Image size
    width  = args.width
    height = args.height

    # Go through all scenes
    for scene_name in scene_list:
        scene_path = os.path.join(data_path, scene_name)

        # Get list of rooms
        rooms_list = get_list_dir(scene_path)

        for room_name in rooms_list:

            room_path = os.path.join(scene_path, room_name)
            print("Processing scene {} - room {} ".format(scene_name, room_name), sep='', end='', flush=True)

            # Create output path
            mkdir_if_not_exist(os.path.join(room_path, "images"))
            mkdir_if_not_exist(os.path.join(room_path, "groundtruth_model"))

            # Read ground truth
            groundtruth = read_groundtruth(room_path, numclasses=38)

            np.savez_compressed(
                    os.path.join(room_path, "groundtruth_model/probs.npz"),
                    probs=groundtruth)

            # Read camera file
            projections, K, R, C = read_projections(room_path, width, height)

            # Read depth maps and labeled images
            depth_maps_list = get_list_files(room_path, args.depth_ext)
            label_maps_list = get_list_files(room_path, args.category_ext)
            
            for idx, depth_map_name in enumerate(depth_maps_list):
                print(".", sep='', end='', flush=True)
                image_id = int(depth_map_name[:6])
                
                # Reading depth map
                depth_map = read_suncg_img(room_path, depth_map_name)
                depth_map /= 1000.0

                # Read label map
                label_maps_name = label_maps_list[idx]
                label_map = read_suncg_img(room_path, label_maps_name, dtype=np.int32)
                label_map -= 1

                # Write the output into one combined NumPy file.
                np.savez_compressed(
                    os.path.join(room_path, "images/{:06d}.npz".format(image_id)),
                    depth_proj_matrix=projections[idx],
                    label_proj_matrix=projections[idx],
                    depth_map=depth_map,
                    label_map=label_map)

                # Visualize depth map back projection (for debugging)
                if args.visualize_proj:
                    points3D = backproj_depth(depth_map, K[idx], R[idx], C[idx])
                    write_ply(os.path.join(room_path, "images/{:06d}.ply".format(image_id)), np.array(points3D), [0, 255, 255])

            print()


if __name__ == "__main__":
    main()