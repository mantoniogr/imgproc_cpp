import cv2
import numpy as np

depth_map = cv2.imread("images/closing_1.png")

height, width, channels = depth_map.shape

depth_map_rgb = np.zeros((height, width, 3))

external = cv2.imread("external_2.png")
internal = cv2.imread("internal_2.png")

for j in range(0, height):
    for i in range(0, width):
        depth_map_rgb[j,i,0] = depth_map[j,i,0]
        depth_map_rgb[j,i,1] = depth_map[j,i,0]
        depth_map_rgb[j,i,2] = depth_map[j,i,0]

        if internal[j,i,0] > 0:
            depth_map_rgb[j,i,2] = 255
        # if internal[j,i,0] > 0:
        #     depth_map_rgb[j,i,2] = 255

cv2.imwrite("result.png", depth_map_rgb)
