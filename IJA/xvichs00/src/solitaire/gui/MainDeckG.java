package solitaire.gui;

import solitaire.model.cards.*;
import solitaire.model.cards.CardDeck;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * Class represents MainDeckG.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class MainDeckG extends CardDeckG {
    /**
     * Card deck.
     */
    public CardDeck deck;

    /**
     * Set card deck.
     * @param deck Card deck.
     */
    public void setDeck(CardDeck deck) {
        this.deck = deck;
        repaint();
    }

    /**
     * Gets deck.
     * @return This deck.
     */
    public CardDeck getDeck() {
        return this.deck;
    }

    /**
     * MainDeckG refference.
     */
    public MainDeckG refference;

    /**
     * Moves top card to restDeck and turning it up on click.
     */
    public MainDeckG() {
        super();
        // Constructor of deck.
        refference = this;
        this.deck = new CardDeck();
        // Adding mouse listener.
        addMouseListener(new MouseAdapter() {

            @Override
            public void mouseClicked(MouseEvent e) {
                // If we click on this deck -> we move top card to restDeck and turn it up.

                // Deletes all active elements.
                parent.activeWorkingDeck = null;
                parent.activeTargetDeck = null;
                parent.activeMainDeckRest = null;

                // Saves snap.
                GameSnap snap = null;
                try {
                    snap = new GameSnap(parent);
                } catch (Exception exception) {

                }

                if (deck.size() > 0) {
                    // Deck is not empty.
                    Card card = deck.pop();
                    if (card != null) {
                        // Saves view for undo.
                        parent.moves.addLast(snap);
                        if (parent.moves.size() > 5) {
                            parent.moves.removeFirst();
                        }

                        // Execute.
                        card.turnFaceUp();
                        parent.mainDeckRestG.deck.put(card);
                        parent.repaintAll();
                    }
                } else {
                    // If deck is empty, move original.
                    CardDeck removedDeck = parent.mainDeckRestG.deck.pop(parent.mainDeckRestG.deck.size());
                    if (removedDeck != null) {
                        // Saves view for undo.
                        parent.moves.addLast(snap);
                        if (parent.moves.size() > 5) {
                            parent.moves.removeFirst();
                        }
                        // Execute.
                        removedDeck.turnDownAll();
                        removedDeck.shuffle();
                        deck.put(removedDeck);
                        parent.repaintAll();
                    }
                }
            }

        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        // Print card on top of the deck.
        Card card = this.deck.get();
        if (!this.deck.isEmpty()) {
            try {
                BufferedImage backImage = ImageIO.read(new File("src/solitaire/img/card_deck.png"));
                BufferedImage frontImage = ImageIO.read(new File("src/solitaire/img/cards/" + card.toString() + ".png"));
                if (card.isTurnedFaceUp()) {
                    g.drawImage(frontImage, 0, 0, this); // see javadoc for more info on the parameters            
                } else {
                    g.drawImage(backImage, 0, 0, this); // see javadoc for more info on the parameters            
                }
            } catch (IOException e) {
                System.out.println("Nepodarilo se nacist obrazek. - MainDeckG");
            }
        }
        if (parent.hintMainDeck == refference) {
            Color red = new Color(255, 0, 0, 100);
            g.setColor(red);
            g.fillRect(0, 0, 59, 80);
            parent.hintMainDeck = null;
        }
    }

}
