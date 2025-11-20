from PIL import Image, ImageDraw, ImageFont

class Screen:
    """
    Abstract base class for all UI screens.
    """
    def __init__(self, context):
        self.context = context
        self.name = "Base Screen"

    def render(self, draw, width, height):
        """
        Render the screen content onto the provided ImageDraw object.
        """
        raise NotImplementedError

    def handle_input(self, key):
        """
        Handle input events.
        Returns True if the screen needs to be re-rendered, False otherwise.
        """
        return False

    def on_enter(self):
        """
        Called when the screen becomes active.
        """
        pass

    def on_exit(self):
        """
        Called when the screen is no longer active.
        """
        pass

class UIContext:
    """
    Manages the application state and current screen.
    """
    def __init__(self, driver):
        self.driver = driver
        self.current_screen = None
        self.running = True
        self.width = 400
        self.height = 300
        if hasattr(driver, 'width'):
            self.width = driver.width
        if hasattr(driver, 'height'):
            self.height = driver.height

    def set_screen(self, screen):
        if self.current_screen:
            self.current_screen.on_exit()
        self.current_screen = screen
        if self.current_screen:
            self.current_screen.on_enter()
        self.refresh()

    def refresh(self):
        if not self.current_screen:
            return

        image = Image.new('1', (self.width, self.height), 255)
        draw = ImageDraw.Draw(image)
        
        self.current_screen.render(draw, self.width, self.height)
        
        # Update display
        self.driver.display(self.driver.getbuffer(image) if hasattr(self.driver, 'getbuffer') else image)

    def handle_input(self, key):
        if self.current_screen:
            if self.current_screen.handle_input(key):
                self.refresh()
