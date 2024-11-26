import argparse
from pathlib import Path

from PIL import Image


def main():
    parser = argparse.ArgumentParser(description="Resize images in a directory.")
    parser.add_argument("--asset_dir", type=Path, help="Target image to resize.")
    parser.add_argument(
        "--width", type=int, default=1024, help="Width of the resized images."
    )
    parser.add_argument(
        "--height", type=int, default=1024, help="Height of the resized images."
    )
    args = parser.parse_args()

    asset_dir = Path(args.asset_dir)
    if not asset_dir.is_dir():
        raise NotADirectoryError(f"Directory not found: {asset_dir}")

    save_dir = asset_dir / "converted"
    save_dir.mkdir(exist_ok=True, parents=True)
    for img_path in asset_dir.glob("*.[pj][np][g]"):  # png or jpg
        output_path = save_dir / img_path.name
        preprocess_img(img_path, output_path, args.width, args.height)


def preprocess_img(input_path, output_path, width=1024, height=1024):
    print(f"Processing image: {input_path}")
    try:
        with Image.open(input_path) as img:
            original_mode = img.mode
            if original_mode not in ["L", "I", "RGB", "RGBA"]:
                raise ValueError(f"Unsupported image mode: {original_mode}")

            resized_img = img.resize((width, height), Image.NEAREST)
            if original_mode == "I":
                print("  - Convert 32-bit grayscale to 8-bit grayscale")
                min_val = resized_img.getextrema()[0]
                max_val = resized_img.getextrema()[1]
                normalized = resized_img.point(
                    lambda x: (x - min_val) / (max_val - min_val) * 255
                )
                resized_img = normalized.convert("L")
            resized_img.save(output_path, format="PNG")
            print(f"  - Converted image saved to: {output_path}")
            print(f"    - Original image size: {img.size}")
            print(f"    - Resized image size: {resized_img.size}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
