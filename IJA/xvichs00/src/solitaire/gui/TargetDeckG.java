package solitaire.gui;

import solitaire.model.cards.*;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * Class represents TargetDeckG.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class TargetDeckG extends CardDeckG {
    /**
     * Target deck.
     */
    public TargetDeck deck;
    /**
     * Target deck refference.
     */
    public TargetDeckG refference;

    /**
     * Sets target deck.
     * @param deck Target deck.
     */
    public void setDeck(TargetDeck deck) {
        this.deck = deck;
        repaint();
    }

    /**
     * Gets target deck.
     * @return Deck.
     */
    public TargetDeck getDeck() {
        return this.deck;
    }

    /**
     * Moving cards to target deck.
     */
    public TargetDeckG() {
        super();
        refference = this;
        // Adding mouse listener.
        addMouseListener(new MouseAdapter() {

            @Override
            public void mouseClicked(MouseEvent e) {

                // Check doubleclick.
                if (refference == parent.activeTargetDeck) {
                    return;
                }

                GameSnap snap = null;
                // Creating deep copy GameWindow.
                try {
                    snap = new GameSnap(parent);
                } catch (Exception exception) {

                }

                if (parent.activeWorkingDeck != null) {
                    // We can move just one card to working deck.
                    Card card = parent.activeWorkingDeck.deck.pop();
                    if (card != null) {
                        if (!deck.put(card)) {
                            // Card put did not happen.
                            parent.activeWorkingDeck.deck.put(card);
                        } else {
                            // Card put happened.
                            // Save view.
                            parent.moves.addLast(snap);
                            if (parent.moves.size() > 5) {
                                parent.moves.removeFirst();
                            }
                            // Turn up top card.
                            if (parent.activeWorkingDeck.deck.size() > 0) {
                                parent.activeWorkingDeck.deck.get().turnFaceUp();
                            }
                            parent.repaintAll();
                        }
                    }
                    parent.activeTargetDeck = null;
                    parent.activeWorkingDeck = null;
                    parent.activeMainDeckRest = null;

                } else if (parent.activeMainDeckRest != null) {
                    // We can move just one card to finish deck.
                    Card card = parent.activeMainDeckRest.deck.pop();
                    if (card != null) {
                        if (!deck.put(card)) {
                            // Card put did not happen.
                            parent.activeMainDeckRest.deck.put(card);
                        } else {
                            // Card put happened.
                            // Save view.
                            parent.moves.addLast(snap);
                            if (parent.moves.size() > 5) {
                                parent.moves.removeFirst();
                            }
                            // Turn up top card.
                            if (parent.activeMainDeckRest.deck.size() > 0) {
                                parent.activeMainDeckRest.deck.get().turnFaceUp();
                            }
                            parent.repaintAll();
                        }
                    }
                    parent.activeTargetDeck = null;
                    parent.activeWorkingDeck = null;
                    parent.activeMainDeckRest = null;

                } else {
                    parent.activeTargetDeck = refference;
                    parent.activeWorkingDeck = null;
                    parent.activeMainDeckRest = null;
                }

                // Check if we win.
                parent.checkWin();

            }

        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        // Prints top card of deck.
        Card card = this.deck.get();
        if (this.deck.isEmpty()) {
            String resource = "src/solitaire/img/cards/";
            if (deck.getColor() == Card.Color.SPADES) {
                resource += "1(S)alpha.png";
            } else if (deck.getColor() == Card.Color.HEARTS) {
                resource += "1(H)alpha.png";
            } else if (deck.getColor() == Card.Color.CLUBS) {
                resource += "1(C)alpha.png";
            } else if (deck.getColor() == Card.Color.DIAMONDS) {
                resource += "1(D)alpha.png";
            }
            try {
                BufferedImage frontImage = ImageIO.read(new File(resource));
                g.drawImage(frontImage, 0, 0, this); // see javadoc for more info on the parameters            
            } catch (IOException e) {
                System.out.println("Nepodarilo se nacist obrazek - TargetDeck");
            }
        } else {
            BufferedImage backImage;
            BufferedImage frontImage;
            try {
                backImage = ImageIO.read(new File("src/solitaire/img/card_deck.png"));
                frontImage = ImageIO.read(new File("src/solitaire/img/cards/" + card.toString() + ".png"));
                if (card.isTurnedFaceUp()) {
                    g.drawImage(frontImage, 0, 0, this); // see javadoc for more info on the parameters            
                } else {
                    g.drawImage(backImage, 0, 0, this); // see javadoc for more info on the parameters            
                }
            } catch (IOException ex) {
                System.out.println("Nepodarilo se nacist obrazek.");
            }
        }

        // Print hint.
        if (parent.hintTargetDeck == refference) {
            g.setColor(new Color(0, 0, 255, 100));
            g.fillRect(0, 0, 59, 80);
            parent.hintTargetDeck = null;
        }
    }
}
