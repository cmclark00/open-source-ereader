import logging
import os
import time
from PIL import Image

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class EPD:
    """
    Abstract base class for E-Paper Display driver.
    This class handles the interaction with the hardware or provides a mock interface.
    """
    def __init__(self):
        self.width = 400
        self.height = 300
        self.is_mock = False

    def init(self):
        raise NotImplementedError

    def clear(self):
        raise NotImplementedError

    def display(self, image):
        raise NotImplementedError

    def sleep(self):
        raise NotImplementedError

class MockEPD(EPD):
    """
    Mock driver for testing on non-Raspberry Pi systems.
    Saves the display output to an image file.
    """
    def __init__(self):
        super().__init__()
        self.width = 400  # Standard 4.2 inch resolution
        self.height = 300
        self.is_mock = True
        logger.info("Initialized Mock EPD Driver")

    def init(self):
        logger.info("Mock EPD: Init")
        return 0

    def clear(self):
        logger.info("Mock EPD: Clear")

    def display(self, image):
        logger.info("Mock EPD: Displaying image")
        # Ensure image is in correct mode and size
        if image.mode != '1':
            image = image.convert('1')
        
        # Save debug image
        debug_path = os.path.join(os.getcwd(), "debug_display_output.png")
        image.save(debug_path)
        logger.info(f"Mock EPD: Saved display content to {debug_path}")

    def sleep(self):
        logger.info("Mock EPD: Sleep")

def get_epd_driver():
    """
    Factory function to get the appropriate driver.
    Tries to import the official waveshare library.
    If it fails (e.g. on Windows), returns the Mock driver.
    """
    try:
        # Try to import the specific 4.2inch driver
        # Note: The actual import path depends on how the waveshare library is installed/structured.
        # This is a common path for the waveshare-epaper package or local clone.
        from waveshare_epd import epd4in2
        logger.info("Successfully imported waveshare_epd.epd4in2")
        return epd4in2.EPD()
    except ImportError:
        logger.warning("Could not import waveshare_epd. Using Mock driver.")
        return MockEPD()
    except Exception as e:
        logger.error(f"Error initializing Waveshare driver: {e}. Using Mock driver.")
        return MockEPD()
