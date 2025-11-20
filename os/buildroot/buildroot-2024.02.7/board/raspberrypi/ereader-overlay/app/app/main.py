import sys
import os
import time
from PIL import Image, ImageDraw, ImageFont

# Add parent directory to path to import drivers
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from drivers.epaper import get_epd_driver

def main():
    print("Starting E-Reader Application...")
    
    # Initialize driver
    epd = get_epd_driver()
    
    try:
        print("Initializing display...")
        epd.init()
        epd.clear()
        
        # Initialize UI Context
        from ui.base import UIContext
        from ui.screens.home import HomeScreen
        from app.input import InputManager
        
        context = UIContext(epd)
        
        # Input callback
        def on_input(key):
            print(f"Input received: {key}")
            context.handle_input(key)
            
        input_manager = InputManager(on_input)
        input_manager.start()
        
        # Set initial screen
        context.set_screen(HomeScreen(context))
        
        print("Application running. Press Ctrl+C to exit.")
        
        # Main loop
        while context.running:
            time.sleep(0.1)
            
        print("Exiting...")
        input_manager.stop()
        epd.sleep()
        
    except IOError as e:
        print(f"IOError: {e}")
    except KeyboardInterrupt:    
        print("Ctrl+C pressed. Exiting...")
        if 'input_manager' in locals():
            input_manager.stop()
        epd.sleep()
        exit()

if __name__ == "__main__":
    main()
