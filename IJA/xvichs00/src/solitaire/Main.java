package solitaire;

import solitaire.gui.*;
import javax.swing.*;

/**
 * Class represents main. created on 24.4.2017
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class Main {

    /**
     * Main method.
     *
     * @param args Program arguments.
     */
    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            /**
             * Runs game grid.
             */
            public void run() {
                GameGrid grid = new GameGrid();
            }
        });
    }

}
