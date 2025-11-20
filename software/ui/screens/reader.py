from PIL import ImageFont
import textwrap
from ..base import Screen

class ReaderScreen(Screen):
    def __init__(self, context, file_path):
        super().__init__(context)
        self.name = "Reader"
        self.file_path = file_path
        self.content = ""
        self.pages = []
        self.current_page = 0
        
        try:
            self.font = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf', 16)
        except IOError:
            try:
                self.font = ImageFont.truetype('arial.ttf', 16)
            except IOError:
                self.font = ImageFont.load_default()

        self.load_book()

    def load_book(self):
        try:
            with open(self.file_path, 'r', encoding='utf-8') as f:
                self.content = f.read()
            self.paginate()
        except Exception as e:
            self.content = f"Error loading book: {e}"
            self.pages = [self.content]

    def paginate(self):
        # Simple pagination logic
        # This is a rough approximation. A real implementation would measure text size properly.
        width = self.context.width
        height = self.context.height
        
        # Margins
        margin_x = 10
        margin_y = 10
        available_width = width - (2 * margin_x)
        available_height = height - (2 * margin_y)
        
        # Estimate chars per line and lines per page
        # This depends heavily on the font.
        # Assuming approx 8 pixels width per char and 20 pixels height per line for size 16 font
        char_width = 8 
        line_height = 20
        
        chars_per_line = available_width // char_width
        lines_per_page = available_height // line_height
        
        lines = []
        for paragraph in self.content.split('\n'):
            if not paragraph.strip():
                lines.append("")
                continue
            wrapped = textwrap.wrap(paragraph, width=chars_per_line)
            lines.extend(wrapped)
            
        self.pages = []
        current_page_lines = []
        
        for line in lines:
            current_page_lines.append(line)
            if len(current_page_lines) >= lines_per_page:
                self.pages.append("\n".join(current_page_lines))
                current_page_lines = []
                
        if current_page_lines:
            self.pages.append("\n".join(current_page_lines))
            
        if not self.pages:
            self.pages = ["Empty book"]

    def render(self, draw, width, height):
        # Draw page content
        if 0 <= self.current_page < len(self.pages):
            text = self.pages[self.current_page]
            draw.text((10, 10), text, font=self.font, fill=0)
            
            # Draw page number
            page_num = f"{self.current_page + 1}/{len(self.pages)}"
            # Get text size to position it at bottom right
            try:
                bbox = draw.textbbox((0, 0), page_num, font=self.font)
                w = bbox[2] - bbox[0]
                h = bbox[3] - bbox[1]
            except AttributeError:
                # Fallback for older Pillow versions
                w, h = draw.textsize(page_num, font=self.font)
                
            draw.text((width - w - 10, height - h - 5), page_num, font=self.font, fill=0)

    def handle_input(self, key):
        if key == 'RIGHT' or key == 'DOWN':
            if self.current_page < len(self.pages) - 1:
                self.current_page += 1
                return True
        elif key == 'LEFT' or key == 'UP':
            if self.current_page > 0:
                self.current_page -= 1
                return True
        elif key == 'BACK' or key == 'SELECT': # Allow Select to exit for now too if Back isn't mapped well
            # Return to Library (or Home for now, we'll fix navigation stack later)
            from .library import LibraryScreen
            # Ideally we should pop from a stack, but for now we just instantiate a new LibraryScreen
            # Or better, go back to the previous screen if we tracked it.
            # Since we don't have a stack yet, let's go to Library.
            self.context.set_screen(LibraryScreen(self.context))
            return True
        return False
