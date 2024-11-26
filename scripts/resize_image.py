import argparse
from pathlib import Path

from PIL import Image


def main():
    parser = argparse.ArgumentParser(description="Resize images in a directory.")
    parser.add_argument("--img_path", type=Path, help="Target image to resize.")
    parser.add_argument(
        "--width", type=int, default=1024, help="Width of the resized images."
    )
    parser.add_argument(
        "--height", type=int, default=1024, help="Height of the resized images."
    )
    args = parser.parse_args()

    img_path = Path(args.img_path)
    save_dir = img_path.parent / "converted"
    save_dir.mkdir(exist_ok=True, parents=True)
    output_path = save_dir / img_path.name
    resize_img(img_path, output_path, args.width, args.height)


def resize_img(input_path, output_path, width=1024, height=1024):
    try:
        with Image.open(input_path) as img:
            resized_img = img.resize((width, height), Image.NEAREST)
            resized_img.save(output_path, format="PNG")
            print(f"Converted image saved to: {output_path}")
            print(f"Original image size: {img.size}")
            print(f"Resized image size: {resized_img.size}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
