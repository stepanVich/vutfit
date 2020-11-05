package solitaire.gui;

import java.awt.*;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.io.*;

/**
 * Class represents GameGrid.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class GameGrid extends JFrame {

    /**
     * Sets up main grid.
     */
    public GameGrid() {
        super("Solitaire Klondike");
        // Setting main frame of app
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Setting size of frame
        setResizable(false);

        // Main layout
        Container pane = getContentPane();
        pane.setLayout(new GridLayout(0, 1));

        // Setting background
        pane.setBackground(Color.GREEN);

        // First window set
        GameWindow gameWindow = new GameWindow();
        gameWindow.setParent(this);

        // Adding first window to frame
        pane.add(gameWindow);

        // Setting visible
        pack();
        setVisible(true);
    }

    /**
     * Saves the game.
     *
     * @param obj Serialized game state.
     */
    public void saveGame(Serializable obj) {
        FileOutputStream fos = null;
        final JFileChooser fc = new JFileChooser();
        fc.setFileFilter(new FileNameExtensionFilter("*.sav files", "sav"));
        int returnValue = fc.showSaveDialog(this);
        try {
            if (returnValue == JFileChooser.APPROVE_OPTION) {
                File file = fc.getSelectedFile();
                fos = new FileOutputStream(file);
                ObjectOutputStream oos = new ObjectOutputStream(fos);
                oos.writeObject(obj);
                oos.flush();
                oos.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Loads the game.
     *
     * @param oldGameWindow Old game window.
     */
    public void loadGame(GameWindow oldGameWindow) {
        final JFileChooser fc = new JFileChooser();
        fc.setFileFilter(new FileNameExtensionFilter("*.sav files", "sav"));
        int returnValue = fc.showOpenDialog(this);
        try {
            if (returnValue == JFileChooser.APPROVE_OPTION) {
                try {
                    ObjectInputStream ois = new ObjectInputStream(new FileInputStream(fc.getSelectedFile()));
                    GameSnap loadGameSnap = (GameSnap) ois.readObject();
                    ois.close();
                    // Replace actual GameWindow with loaded GameWindow
                    oldGameWindow.replaceDecks(loadGameSnap);
                } catch (StreamCorruptedException e) {
                    System.out.println("Soubor se nepodarilo precist.");
                }
            }

        } catch (ClassNotFoundException | IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Adds new game.
     */
    public void addTable() {
        // Helper for number of opened game windows
        int openWin = getContentPane().getComponentCount();
        // Cheks number of games.
        if (openWin >= 4) {
            return;
        }

        // Grid layout change
        if (openWin >= 1) {
            getContentPane().setLayout(new GridLayout(0, 2));
        } else {
            getContentPane().setLayout(new GridLayout(0, 1));
        }

        // Window add
        GameWindow gameWindow = new GameWindow();
        gameWindow.setParent(this);
        getContentPane().add(gameWindow);
        pack();
    }

    /**
     * Closes game table.
     *
     * @param comp Game
     */
    public void closeTable(Component comp) {

        // Helper for number of opened game windows
        int openWin = getContentPane().getComponentCount();

        // If it's last window, app'll end.
        if (openWin == 1) {
            // Optimized for the best acceleration.
            setVisible(false);
            System.exit(0);
        }

        // Layout change
        if (openWin == 2) {
            getContentPane().setLayout(new GridLayout(0, 1));
        }

        // Removing component (game)
        remove(comp);
        pack();
        repaint();
    }

}
