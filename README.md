
# Parking Management System (PMS)

## Description

This system uses a combination of **computer vision**, **machine learning**, and **Arduino-based hardware** to automate parking management. It detects vehicle license plates using a pre-trained YOLO model, captures images of detected plates, logs the plate information in a CSV file, and controls an Arduino gate to allow access to vehicles. It simulates an ultrasonic sensor to control the flow of vehicle entry.

## Features

- **License Plate Recognition (LPR):** Using YOLOv8 for plate detection.
- **Ultrasonic Sensor Simulation:** Simulated distance measurement to trigger plate detection.
- **Arduino Integration:** Controls a gate based on detected plates.
- **CSV Logging:** Logs detected plates and payment status.
- **Image Saving:** Saves detected plate images in a directory.
- **Tesseract OCR:** Used for reading text from detected license plates.

## Requirements

1. **Python 3.x** (Recommended: Python 3.7+)
2. **Required Libraries:**
   - OpenCV
   - ultralytics (YOLOv8)
   - pytesseract
   - pyserial
   - numpy
   - collections

You can install the required libraries using pip:

```bash
pip install opencv-python ultralytics pytesseract pyserial numpy
```

3. **Tesseract OCR:**
   - Download and install [Tesseract OCR](https://github.com/tesseract-ocr/tesseract) for license plate text recognition.
   - Make sure to set the path to the Tesseract executable in the code.

   Example:
   ```python
   pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR	esseract.exe'
   ```

4. **Arduino**:
   - The code interacts with an Arduino connected via USB to control a gate.
   - Ensure your Arduino is connected and identified as a serial device (typically COM port).

## Setup

### 1. Clone this repository:

```bash
git clone https://github.com/castella23/PMS.git
cd PMS
```

### 2. Prepare the Model:

- Ensure you have the **best.pt** file (the trained YOLO model) in the `parking-management-system` folder or update the code with the correct path to your model.

### 3. Run the System:

```bash
python car_entry.py
```

### 4. Hardware Setup:

- Connect your **Arduino** device and ensure it is detected by the system.
- The system will attempt to open the gate when a valid plate is detected.

## Usage

1. The system will continuously monitor the camera for license plates.
2. When a plate is detected, the system will log the plate number along with the timestamp to `plates_log.csv`.
3. The gate will open if a valid plate is detected and then close after a brief delay.
4. Press **'q'** to quit the system.

## File Structure

```plaintext
PMS/
├── car_entry.py            # Main entry point script for plate detection and gate control.
├── plates_log.csv          # Logs the detected plate numbers and timestamps.
├── parking-management-system/
│   ├── best.pt             # YOLOv8 model weights for plate detection.
│   ├── plates/             # Directory where detected plate images are saved.
│   └── ...
└── README.md               # This file.
```

## Contributing

Feel free to fork this repository, submit issues, and create pull requests. Contributions are welcome!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
