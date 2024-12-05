from pathlib import Path

from PIL import Image

# _ALLOWED_SIZES = [1024, 2048, 4096, 8192]
_ALLOWED_SIZES = [1024, 2048]
# _ALLOWED_SIZES = [1024]


def main():
    base_dir = Path(__file__).parent.parent / "assets" / "Terrain"
    for asset_dir in base_dir.glob("*"):
        if not asset_dir.is_dir():
            continue

        print(f"Processing asset directory: {asset_dir}")
        for img_path in asset_dir.glob("*.[pj][np][g]"):
            preprocess_img(img_path)


def preprocess_img(img_path):
    print(f"Processing image: {img_path}")
    try:
        with Image.open(img_path) as img:
            original_mode = img.mode
            if original_mode not in ["L", "I", "P", "RGB", "RGBA"]:
                raise ValueError(f"Unsupported image mode: {original_mode}")

            # find the nearest allowed size
            original_width, original_height = img.size
            width = min(_ALLOWED_SIZES, key=lambda x: abs(x - original_width))
            height = min(_ALLOWED_SIZES, key=lambda x: abs(x - original_height))

            # preprocess the image
            processed_img = img.resize((width, height), Image.NEAREST)
            if original_mode == "I":
                print("  - Convert 32-bit grayscale to 8-bit grayscale")
                min_val = processed_img.getextrema()[0]
                max_val = processed_img.getextrema()[1]
                processed_img = processed_img.point(
                    lambda x: (x - min_val) / (max_val - min_val) * 255
                )
            if original_mode in ["I", "L"]:
                print("  - Convert grayscale to RGB image")
                processed_img = processed_img.convert("RGB")
            if original_mode == "P":
                print("  - Convert palette image to RGB image")
                processed_img = processed_img.convert("RGB")

            save_dir = img_path.parent / "converted"
            save_dir.mkdir(exist_ok=True, parents=True)
            save_path = save_dir / img_path.name
            processed_img.save(save_path, format="PNG")
            print(f"  - Converted image saved to: {save_path}")
            print(f"    - Original image size: {img.size}")
            print(f"    - Resized image size: {processed_img.size}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
