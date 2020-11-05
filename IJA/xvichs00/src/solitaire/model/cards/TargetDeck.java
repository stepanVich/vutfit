package solitaire.model.cards;

/**
 * Class for target deck.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class TargetDeck extends CardDeck {

    /**
     * Color of the target deck.
     */
    protected Card.Color color;

    /**
     * Gets color of the deck.
     *
     * @return Color of the deck.
     */
    public Card.Color getColor() {
        return this.color;
    }

    /**
     * Gets color of the deck.
     *
     * @param color Color of the deck.
     */
    public TargetDeck(Card.Color color) {
        super();
        this.color = color;
    }

    /**
     * Appends the specified card to the end of this list. Operation will
     * succeed if top card is same color, turned up and card is higer by 1.
     *
     * @param card Specified card.
     * @return True if operation was succesful. Otherwise false.
     */
    @Override
    public boolean put(Card card) {
        // Color check
        if (card.color() != this.color) {
            return false;
        }

        // Flip check
        if (!card.isTurnedFaceUp()) {
            return false;
        }

        // Sequence check
        if (this.size() > 0) {
            // Cards in stack -> Checks with card on top.
            Card topCard = this.get();
            if (card.compareValue(topCard) != 1) {
                return false;
            }
        } else {
            // No cards in stack.
            if (card.value() != 1) {
                return false;
            }
        }

        // All checks are OK -> Add card on top.
        this.deck.addLast(card);
        return true;
    }
}
