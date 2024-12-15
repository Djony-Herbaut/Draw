import tkinter as tk
import math

class DrawApp:
    def __init__(self, width=800, height=600, bg="white"):
        self.root = tk.Tk()
        self.root.title("Canvas Draw")
        self.canvas = tk.Canvas(self.root, width=width, height=height, bg=bg)
        self.canvas.pack()

        # Attributs du curseur
        self.cursor = self.canvas.create_oval(390, 290, 410, 310, fill="black")
        self.cursor_x, self.cursor_y = 400, 300
        self.pen_down = True  # Stylo abaissé
        self.cursor_color = "black"
        self.pen_size = 1

    def start(self):
        """Lance la boucle principale de l'application."""
        self.root.mainloop()

    def drawsetpos(self, x, y):
        """Positionne le curseur à des coordonnées absolues."""
        if self.pen_down:
            self.canvas.create_line(self.cursor_x, self.cursor_y, x, y, fill=self.cursor_color, width=self.pen_size)
        self.canvas.coords(self.cursor, x-10, y-10, x+10, y+10)
        self.cursor_x, self.cursor_y = x, y

    def drawgo(self, x, y):
        """Positionne le curseur sans dessiner."""
        self.pen_down = False
        self.drawsetpos(x, y)
        self.pen_down = True

    def drawsetx(self, x):
        """Positionne le curseur sur l'axe X uniquement."""
        self.drawsetpos(x, self.cursor_y)

    def drawsety(self, y):
        """Positionne le curseur sur l'axe Y uniquement."""
        self.drawsetpos(self.cursor_x, y)

    def drawshowcursor(self):
        """Affiche le curseur."""
        self.canvas.itemconfigure(self.cursor, state="normal")

    def drawhidecursor(self):
        """Masque le curseur."""
        self.canvas.itemconfigure(self.cursor, state="hidden")

    def drawcursorcolor(self, colorname):
        """Change la couleur du curseur et du trait."""
        self.cursor_color = colorname
        self.canvas.itemconfigure(self.cursor, fill=colorname)

    def drawpensize(self, width):
        """Change l'épaisseur du trait."""
        self.pen_size = width

    def drawmoveforward(self, distance):
        """Déplace le curseur vers l'avant."""
        angle = math.radians(0)  # Par défaut, orienté à droite
        x = self.cursor_x + distance * math.cos(angle)
        y = self.cursor_y + distance * math.sin(angle)
        self.drawsetpos(x, y)

    def drawmovebackward(self, distance):
        """Déplace le curseur vers l'arrière."""
        self.drawmoveforward(-distance)

    def drawpivotleft(self, angle):
        """Tourne le curseur à gauche."""
        pass  # À implémenter avec un état d'orientation

    def drawpivotright(self, angle):
        """Tourne le curseur à droite."""
        pass  # À implémenter avec un état d'orientation

    def drawpendown(self):
        """Abaisse le stylo."""
        self.pen_down = True

    def drawpenup(self):
        """Lève le stylo."""
        self.pen_down = False

    def drawcircle(self, radius):
        """Dessine un cercle à partir du curseur."""
        x, y = self.cursor_x, self.cursor_y
        self.canvas.create_oval(x-radius, y-radius, x+radius, y+radius, outline=self.cursor_color, width=self.pen_size)

    def drawdot(self, size, color):
        """Dessine un point à la position du curseur."""
        x, y = self.cursor_x, self.cursor_y
        self.canvas.create_oval(x-size/2, y-size/2, x+size/2, y+size/2, fill=color, outline=color)

    def drawarc(self, radius, extent):
        """Dessine un arc."""
        x, y = self.cursor_x, self.cursor_y
        self.canvas.create_arc(x-radius, y-radius, x+radius, y+radius, start=0, extent=extent, outline=self.cursor_color, width=self.pen_size)

    def drawspeed(self, speed):
        """Change la vitesse de déplacement."""
        pass  # Peut être utilisé avec des animations tkinter

    def drawtracer(self, n, delay):
        """Active ou désactive le traçage."""
        self.canvas.update_idletasks()

    def drawupdate(self):
        """Met à jour l'écran."""
        self.canvas.update()

    def drawclearscreen(self):
        """Efface tout le canvas."""
        self.canvas.delete("all")

    def drawshape(self, shape):
        """Change la forme du curseur."""
        pass  # Implémentation possible avec des images ou des polygones

if __name__ == "__main__":
    app = DrawApp()
    app.drawsetpos(200, 200)
    app.drawcircle(50)
    app.drawdot(10, "red")
    app.drawmoveforward(100)
    app.drawpendown()
    app.drawarc(100, 90)
    #app.drawclearscreen()
    app.start()
