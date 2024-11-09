import os
import numpy as np
import matplotlib.pyplot as plt
import argparse

def convert_depth_to_image(folder_path):

    output_folder_path = os.path.join(folder_path, 'Preview')
    if not os.path.exists(output_folder_path):
        os.makedirs(output_folder_path)

    # Get a list of all .npy files in the directory
    npy_files = [f for f in os.listdir(folder_path) if f.endswith('.npy')]

    min_value = 0
    max_value = 2000 # set max distance to avoid infinite distance of the background

    for npy_file in npy_files:
        # Construct the full file path
        file_path = os.path.join(folder_path, npy_file)
        
        # Load the 2D numpy array from the .npy file
        data = np.load(file_path)

        # Handle infinite and NaN values
        data = np.nan_to_num(data, nan=max_value, posinf=max_value, neginf=min_value)

        # Clip the data to the desired range
        data = np.clip(data, min_value, max_value)
        
        # Create a figure and plot the 2D data
        plt.figure(figsize=(18, 32))
        plt.imshow(data, cmap='gray')  # You can choose a different colormap if you prefer
        plt.axis('off')  # Hide axis ticks and labels
        
        # Save the figure as an image file (e.g., .png)
        image_filename = os.path.splitext(npy_file)[0] + '.png'
        image_path = os.path.join(output_folder_path, image_filename)
        plt.savefig(image_path, bbox_inches='tight', pad_inches=0)
        plt.close()  # Close the figure to free up memory

        print(f"Generated image for {npy_file} -> {image_filename}")

if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Convert segmentation images to color using a color dictionary.")
    parser.add_argument("-p", "--path", type=str, required=True, help="Path to the folder containing segmentation images.")
    args = parser.parse_args()

    # Call the function
    convert_depth_to_image(args.path)