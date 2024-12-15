import tkinter as tk
import threading
import sys

class DrawApp:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Interactive Canvas")
        self.canvas = tk.Canvas(self.root, width=800, height=600, bg="white")
        self.canvas.pack()

        # Initialisation du curseur
        self.cursor = self.canvas.create_oval(390, 290, 410, 310, fill="black")
        self.cursor_x, self.cursor_y = 400, 300
        self.pen_down = True  # Stylo abaissé
        self.cursor_color = "black"
        self.pen_size = 1

        # Thread pour écouter les instructions
        self.running = True
        threading.Thread(target=self.listen_instructions, daemon=True).start()

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
        x = self.cursor_x + distance
        y = self.cursor_y
        self.drawsetpos(x, y)

    def drawmovebackward(self, distance):
        """Déplace le curseur vers l'arrière."""
        self.drawmoveforward(-distance)

    def drawmoveupward(self, distance):
        """Déplace le curseur vers le haut."""
        x = self.cursor_x
        y = self.cursor_y + distance
        self.drawsetpos(x, y)

    def drawmovedownward(self, distance):
        """Déplace le curseur vers le bas."""
        self.drawmoveupward(-distance)

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
        self.cursor_x, self.cursor_y = 400, 300
        self.canvas.coords(self.cursor, 390, 290, 410, 310)

    def listen_instructions(self):
        """Écoute des instructions en temps réel depuis stdin."""
        while self.running:
            try:
                line = sys.stdin.readline().strip()
                if not line:
                    continue
                print(f"Instruction reçue : {line}")

                # Interprétation des commandes
                if line == "quit":
                    self.running = False
                    self.root.quit()

                # Déplacement du curseur
                elif line.startswith("pos"):
                    _, x, y = line.split()
                    self.drawsetpos(int(x), int(y))
                elif line.startswith("go"):
                    _, x, y = line.split()
                    self.drawgo(int(x), int(y))
                elif line.startswith("setx"):
                    _, x = line.split()
                    self.drawsetx(int(x))
                elif line.startswith("sety"):
                    _, y = line.split()
                    self.drawsety(int(y))
                elif line.startswith("forward"):
                    _, distance = line.split()
                    self.drawmoveforward(int(distance))
                elif line.startswith("backward"):
                    _, distance = line.split()
                    self.drawmovebackward(int(distance))
                elif line.startswith("upward"):
                    _, distance = line.split()
                    self.drawmoveupward(int(distance))
                elif line.startswith("downward"):
                    _, distance = line.split()
                    self.drawmovedownward(int(distance))

                # Curseur
                elif line == "showcursor":
                    self.drawshowcursor()
                elif line == "hidecursor":
                    self.drawhidecursor()
                elif line.startswith("cursorcolor"):
                    _, colorname = line.split()
                    self.drawcursorcolor(colorname)

                # Stylo
                elif line == "penup":
                    self.drawpenup()
                elif line == "pendown":
                    self.drawpendown()
                elif line.startswith("pensize"):
                    _, width = line.split()
                    self.drawpensize(int(width))

                # Formes
                elif line.startswith("circle"):
                    _, radius = line.split()
                    self.drawcircle(int(radius))
                elif line.startswith("dot"):
                    _, size, color = line.split()
                    self.drawdot(int(size), color)
                elif line.startswith("arc"):
                    _, radius, extent = line.split()
                    self.drawarc(int(radius), int(extent))

                # Pivot
                elif line.startswith("pivotleft"):
                    _, angle = line.split()
                    self.drawpivotleft(int(angle))  # Implémentation future
                elif line.startswith("pivotright"):
                    _, angle = line.split()
                    self.drawpivotright(int(angle))  # Implémentation future

                # Écran et animation
                elif line == "clear":
                    self.drawclearscreen()
                elif line.startswith("speed"):
                    _, speed = line.split()
                    self.drawspeed(int(speed))  # Implémentation future
                elif line.startswith("tracer"):
                    _, n, delay = line.split()
                    self.drawtracer(int(n), int(delay))  # Implémentation future
                elif line == "update":
                    self.drawupdate()

                else:
                    print(f"Commande non reconnue : {line}")
            except Exception as e:
                print(f"Erreur dans la commande : {e}")

    def start(self):
        self.root.mainloop()

if __name__ == "__main__":
    app = DrawApp()
    app.start()
