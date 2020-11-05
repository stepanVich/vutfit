package solitaire.gui;

import solitaire.model.cards.*;
import solitaire.model.cards.CardDeck;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * Class represents WorkingDeckG.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class WorkingDeckG extends CardDeckG {

    /**
     * Working deck.
     */
    public WorkingDeck deck;

    /**
     * Sets working deck.
     * @param deck Working deck.
     */
    public void setDeck(WorkingDeck deck) {
        this.deck = deck;
        repaint();
    }

    /**
     * Gets working deck.
     * @return Working deck.
     */
    public WorkingDeck getDeck() {
        return this.deck;
    }
    /**
     * Card offset.
     */
    protected int card_offset = 20;
    /**
     * Working deck refference.
     */
    protected WorkingDeckG refference;

    /**
     * Handles working deck.
     */
    public WorkingDeckG() {
        super();
        refference = this;
        // Setting up special size.
        setPreferredSize(new Dimension(60, 213));
        //setPreferredSize(new Dimension(60, 300));
        
        // Adding mouse listener.
        addMouseListener(new MouseAdapter() {

            @Override
            public void mouseClicked(MouseEvent e) {
                boolean successMove = false;
                // Checks double click, if we click two times or more, do nothing.
                if (refference == parent.activeWorkingDeck) {
                    return;
                }

                // Creating deep copy of GameWindow.
                GameSnap snap = null;
                try {
                    snap = new GameSnap(parent);
                } catch (Exception exception) {

                }

                // Checks input.
                if (parent.activeMainDeckRest != null) {
                    // RestDect -> WorkingDeck
                    Card card = parent.activeMainDeckRest.deck.pop();
                    if (card != null) {

                        if (!deck.put(card)) {
                            // Card put did not happen.
                            parent.activeMainDeckRest.deck.put(card);
                        } else {
                            // Card put happened.
                            successMove = true;
                            // Recolor decks.
                            parent.moves.addLast(snap);
                            if (parent.moves.size() > 5) {
                                parent.moves.removeFirst();
                            }
                            parent.repaintAll();
                        }
                    }
                } else if (parent.activeWorkingDeck != null) {
                    // WorkingDeck -> WorkingDeck
                    int count = parent.activeWorkingDeck.deck.size() - parent.workingDeckCardIndex;
                    if (count <= 0) {
                        count = 1;
                    }
                    CardDeck selectedDeck = parent.activeWorkingDeck.deck.pop(count);
                    if (selectedDeck != null) {
                        if (!deck.put(selectedDeck)) {
                            // Deck put did not happened.
                            // Put cards back.
                            parent.activeWorkingDeck.deck.put(selectedDeck);
                        } else {
                            // Card put happened.
                            successMove = true;
                            parent.moves.addLast(snap);
                            if (parent.moves.size() > 5) {
                                parent.moves.removeFirst();
                            }
                            // Turn up top card of working pack from which we moved deck (stack).
                            if (parent.activeWorkingDeck.deck.size() > 0) {
                                parent.activeWorkingDeck.deck.get().turnFaceUp();
                            }
                            parent.repaintAll();
                        }
                    }
                } else if (parent.activeTargetDeck != null) {
                    // TargetDeck -> WorkingDeck
                    Card card = parent.activeTargetDeck.deck.pop();
                    if (card != null) {
                        if (!deck.put(card)) {
                            // Deck put did not happened.
                            parent.activeTargetDeck.deck.put(card);
                        } else {
                            // Card put happened.
                            successMove = true;
                            // Saves view for undo.
                            parent.moves.addLast(snap);
                            if (parent.moves.size() > 5) {
                                parent.moves.removeFirst();
                            }
                            // Recolor decks.
                            parent.repaintAll();
                        }
                    }
                }
                // Parse input.
                if (!successMove) {
                    parent.activeWorkingDeck = refference;
                    parent.workingDeckCardIndex = (int) (e.getY() / card_offset);
                } else {
                    parent.activeWorkingDeck = null;
                }
                parent.activeTargetDeck = null;
                parent.activeMainDeckRest = null;

                parent.checkWin();
            }

        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        // Calculate offset of cards according to number of cards.
        if (!deck.isEmpty()) {
            card_offset = ((213 - 5) - 81) / deck.size();
            if (card_offset > 20) {
                card_offset = 20;
            }
        }

        // Prints top card of deck.
        if (!this.deck.isEmpty()) {

            // Print working deck.
            BufferedImage backImage;
            BufferedImage frontImage;
            for (int i = 0; i < deck.size(); i++) {
                Card card = deck.get(i);
                try {
                    backImage = ImageIO.read(new File("src/solitaire/img/card_deck.png"));
                    frontImage = ImageIO.read(new File("src/solitaire/img/cards/" + card.toString() + ".png"));
                    if (card.isTurnedFaceUp()) {
                        g.drawImage(frontImage, 0, i * card_offset, this); // see javadoc for more info on the parameters            
                    } else {
                        g.drawImage(backImage, 0, i * card_offset, this); // see javadoc for more info on the parameters            
                    }
                } catch (IOException ex) {
                    System.out.println("Nepodarilo se nacist obrazek. - WorkingDeckG");
                }
            }
        }
        if (parent.hintWorkingDeck == refference) {
            Color color = new Color(255, 0, 0, 100);
            g.setColor(color);
            g.fillRect(0, card_offset * parent.hintIndexOfCard, 59, 87 + (deck.size() - parent.hintIndexOfCard - 1) * card_offset);
            parent.hintWorkingDeck = null;
        }
        if (parent.hintWorkingDeckTo == refference) {
            Color blue = new Color(0, 0, 255, 100);
            g.setColor(blue);
            g.fillRect(0, card_offset * (deck.size() - 1), 59, 87);
            parent.hintWorkingDeckTo = null;
        }

    }
}
