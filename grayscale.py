import cv2
import numpy as np
import os

def convert_to_grayscale(input_path, output_path):
    """
    Convert an image to grayscale and save it
    
    Args:
        input_path (str): Path to input image
        output_path (str): Path where grayscale image will be saved
    """
    # Read the image
    img = cv2.imread(input_path)
    
    # Convert to grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Save the grayscale image
    cv2.imwrite(output_path, gray)

def batch_convert_directory(input_dir, output_dir):
    """
    Convert all images in a directory to grayscale
    
    Args:
        input_dir (str): Directory containing input images
        output_dir (str): Directory where grayscale images will be saved
    """
    # Create output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Process each file in the input directory
    for filename in os.listdir(input_dir):
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
            input_path = os.path.join(input_dir, filename)
            output_path = os.path.join(output_dir, f"gray_{filename}")
            convert_to_grayscale(input_path, output_path)

def save_as_pgm(input_path, output_path):
    img = cv2.imread(input_path, cv2.IMREAD_GRAYSCALE)
    with open(output_path, 'wb') as f:
        f.write(f'P5\n{img.shape[1]} {img.shape[0]}\n255\n'.encode())
        img.tofile(f)
