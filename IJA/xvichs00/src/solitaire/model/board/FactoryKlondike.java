package solitaire.model.board;

import solitaire.model.cards.*;

/**
 * Factory for creating cards and decks.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class FactoryKlondike {

    /**
     * Creates new card.
     *
     * @param color Card color.
     * @param value Card value.
     * @return Created card.
     */
    public Card createCard(Card.Color color, int value) {
        if (value < 1 || value > 13) {
            return null;
        }
        return new Card(color, value);
    }

    /**
     * Creates new card deck.
     *
     * @return New deck of cards.
     */
    public CardDeck createCardDeck() {
        return createStandardDeck();
    }

    /**
     * Creates new standard deck.
     *
     * @return New standard deck.
     */
    public CardDeck createStandardDeck() {
        CardDeck deck = new CardDeck();
        for (int i = 1; i <= 13; i++) {
            deck.put(new Card(Card.Color.CLUBS, i));
        }
        for (int i = 1; i <= 13; i++) {
            deck.put(new Card(Card.Color.DIAMONDS, i));
        }
        for (int i = 1; i <= 13; i++) {
            deck.put(new Card(Card.Color.HEARTS, i));
        }
        for (int i = 1; i <= 13; i++) {
            deck.put(new Card(Card.Color.SPADES, i));
        }
        return deck;

    }

    /**
     * Creates new target deck by specified color.
     *
     * @param color Color of target deck.
     * @return New target deck by specified color.
     */
    public TargetDeck createTargetDeck(Card.Color color) {
        return new TargetDeck(color);
    }

    /**
     * Creates new working deck.
     *
     * @return New working deck.
     */
    public WorkingDeck createWorkingDeck() {
        return new WorkingDeck();
    }

}
