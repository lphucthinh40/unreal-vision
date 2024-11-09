import os
import cv2
import numpy as np
from PIL import Image
import argparse

def generate_color_dict():
    colors = {}
    index = 0

    # Iterate through the possible values for Red, Green, and Blue channels
    for red in range(0, 256, 32):  # 256 / 8 = 32 step size for Red (0 to 224)
        for blue in range(0, 256, 64):  # 256 / 4 = 64 step size for Blue (0 to 192)
            for green in range(0, 256, 32):  # 256 / 8 = 32 step size for Green (0 to 224) -- our eyes are more sensitive to green
                # Create a hex color code
                colors[index] = [red, green, blue]
                index += 1

    return colors

def convert_segmentation_to_color(folder_path, color_dict):
    output_folder_path = os.path.join(folder_path, 'Preview')
    if not os.path.exists(output_folder_path):
        os.makedirs(output_folder_path)
    # Iterate through all .png files in the folder
    for file_name in os.listdir(folder_path):
        if file_name.endswith('.png'):
            # Read the segmentation image
            image_path = os.path.join(folder_path, file_name)
            segmentation_image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)
            
            # Create a blank RGB image with the same dimensions as the input image
            height, width = segmentation_image.shape[:2]
            colored_image = np.zeros((height, width, 3), dtype=np.uint8)

            # Map each object id to the corresponding color from the dictionary
            for object_id, color in color_dict.items():
                
                # Create a mask for the current object id
                mask = segmentation_image[:, :, 2] == object_id
                
                # Apply the color to the masked regions
                colored_image[mask] = color

            # Save the new image in the same folder, overwriting the original file
            output_path = os.path.join(output_folder_path, file_name)
            Image.fromarray(colored_image).save(output_path)
            print(f"Generate colored image at {output_path}")

if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Convert segmentation images to color using a color dictionary.")
    parser.add_argument("-p", "--path", type=str, required=True, help="Path to the folder containing segmentation images.")
    args = parser.parse_args()

    # Color dictionary (insert the one created in the previous step)
    color_dict = generate_color_dict()

    # Call the function
    convert_segmentation_to_color(args.path, color_dict)