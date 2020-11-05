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
 * Class represents MainDeckRestG.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class MainDeckRestG extends CardDeckG {
	/**
      * Card deck.
      */
	public CardDeck deck;
	
	/**
      * Sets card deck.
      * @param deck Card deck.
      */
	public void setDeck(CardDeck deck) {
		this.deck = deck;
		repaint();
	}

	/**
      * Gets card deck.
      * @return Card deck.
      */
	public CardDeck getDeck() {
		return this.deck;
	}

	/**
      * Card deck refference.
      */
	public MainDeckRestG refference;

	/**
	 * Rest of mouse clicks.
	 */
	public MainDeckRestG() {
		super();
		refference = this;
		// Adding mouse listener.
		addMouseListener(new MouseAdapter() {



			@Override
			public void mouseClicked(MouseEvent e) {
				if(refference == parent.activeMainDeckRest) return;
				parent.activeMainDeckRest = refference;				
				parent.activeTargetDeck = null;
				parent.activeWorkingDeck = null;	
			}

		});
	}


	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		// Prints top card of deck.
		Card card = this.deck.get();
		if(!this.deck.isEmpty()) {
			BufferedImage backImage;
			BufferedImage frontImage;
			try {                
				backImage = ImageIO.read(new File("src/solitaire/img/card_deck.png"));
				frontImage = ImageIO.read(new File("src/solitaire/img/cards/" + card.toString() + ".png"));
				if(card.isTurnedFaceUp()) {
					g.drawImage(frontImage, 0, 0, this); // see javadoc for more info on the parameters            
				} else {
					g.drawImage(backImage, 0, 0, this); // see javadoc for more info on the parameters            
				}
			} catch (IOException ex) {
				System.out.println("Nepodarilo se nacist obrazek.");
			}
		}
		if(parent.hintMainDeckRest == refference) {
			Color red = new Color(255,0,0, 100);
			g.setColor(red);
			g.fillRect(0, 0, 59, 80);
			parent.hintMainDeckRest = null;
		}

	}
}
