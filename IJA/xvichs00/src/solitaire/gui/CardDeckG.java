package solitaire.gui;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.Dimension;
import javax.swing.JPanel;

/**
 * Class represents CardDeckGUI
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class CardDeckG extends JPanel {

    /**
     * Game window.
     */
    public GameWindow parent;

    /**
     * Sets game parent.
     *
     * @param parent Parent.
     */
    public void setParent(GameWindow parent) {
        this.parent = parent;
    }

    /**
     * Constructor.
     */
    public CardDeckG() {
        super();

        // Set size of stack.
        setPreferredSize(new Dimension(60, 81));
        setVisible(true);

        // Background set.
        setBackground(Color.GREEN);

    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        setBackground(Color.GREEN);
    }
}
