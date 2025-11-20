from PIL import ImageFont
from ..base import Screen

class HomeScreen(Screen):
    def __init__(self, context):
        super().__init__(context)
        self.name = "Home"
        self.menu_items = ["Library", "Settings", "About", "Exit"]
        self.selected_index = 0
        
        try:
            self.font_title = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf', 24)
            self.font_item = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf', 18)
        except IOError:
            try:
                self.font_title = ImageFont.truetype('arial.ttf', 24)
                self.font_item = ImageFont.truetype('arial.ttf', 18)
            except IOError:
                self.font_title = ImageFont.load_default()
                self.font_item = ImageFont.load_default()

    def render(self, draw, width, height):
        # Draw title
        draw.text((10, 10), "Open E-Reader", font=self.font_title, fill=0)
        draw.line((10, 40, width - 10, 40), fill=0)

        # Draw menu items
        start_y = 60
        item_height = 30
        
        for i, item in enumerate(self.menu_items):
            y = start_y + (i * item_height)
            prefix = "> " if i == self.selected_index else "  "
            draw.text((20, y), f"{prefix}{item}", font=self.font_item, fill=0)

    def handle_input(self, key):
        if key == 'UP':
            self.selected_index = (self.selected_index - 1) % len(self.menu_items)
            return True
        elif key == 'DOWN':
            self.selected_index = (self.selected_index + 1) % len(self.menu_items)
            return True
        elif key == 'SELECT':
            selected = self.menu_items[self.selected_index]
            print(f"Selected: {selected}")
            if selected == "Library":
                from .library import LibraryScreen
                self.context.set_screen(LibraryScreen(self.context))
            elif selected == "Exit":
                self.context.running = False
            return True
        return False
