import os
from PIL import ImageFont
from ..base import Screen
from .reader import ReaderScreen

class LibraryScreen(Screen):
    def __init__(self, context):
        super().__init__(context)
        self.name = "Library"
        self.books_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), 'books')
        self.books = self.get_books()
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

    def get_books(self):
        books = []
        if os.path.exists(self.books_dir):
            for f in os.listdir(self.books_dir):
                if f.endswith('.txt'):
                    books.append(f)
        if not books:
            books = ["No books found"]
        return books

    def render(self, draw, width, height):
        # Draw title
        draw.text((10, 10), "Library", font=self.font_title, fill=0)
        draw.line((10, 40, width - 10, 40), fill=0)

        # Draw book list
        start_y = 60
        item_height = 30
        
        # Simple scrolling could be added here, but for now just list first few
        max_items = (height - start_y) // item_height
        
        start_index = 0
        if self.selected_index >= max_items:
            start_index = self.selected_index - max_items + 1
            
        visible_books = self.books[start_index : start_index + max_items]
        
        for i, book in enumerate(visible_books):
            y = start_y + (i * item_height)
            real_index = start_index + i
            prefix = "> " if real_index == self.selected_index else "  "
            draw.text((20, y), f"{prefix}{book}", font=self.font_item, fill=0)

    def handle_input(self, key):
        if key == 'UP':
            self.selected_index = (self.selected_index - 1) % len(self.books)
            return True
        elif key == 'DOWN':
            self.selected_index = (self.selected_index + 1) % len(self.books)
            return True
        elif key == 'SELECT':
            selected_book = self.books[self.selected_index]
            if selected_book != "No books found":
                file_path = os.path.join(self.books_dir, selected_book)
                self.context.set_screen(ReaderScreen(self.context, file_path))
            return True
        elif key == 'BACK':
            from .home import HomeScreen
            self.context.set_screen(HomeScreen(self.context))
            return True
        return False
