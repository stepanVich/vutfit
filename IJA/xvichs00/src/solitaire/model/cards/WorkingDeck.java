package solitaire.model.cards;

/**
 * Class for working pack.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class WorkingDeck extends CardDeck {

    /**
     * Constructor
     */
    public WorkingDeck() {
        super();
    }

    /**
     * When stack is empty and card is turned down or has value = 13 (and is
     * turned up) then is added on top. If stack is not empty, card is added if
     * top card is not similar color, turned up and cards value is lower by 1.
     *
     * @param card Card to add.
     * @return Returns true if operation was succesful.
     */
    public boolean put(Card card) {
        // Sequence check
        if (this.size() > 0) {
            // Cards in stack -> Checks with card on top.
            Card topCard = this.get();
            // Card on top is turned up.
            if (topCard.isTurnedFaceUp()) {
                // Check straight
                if (card.compareValue(topCard) != -1) {
                    return false;
                }
                // Checks similar color - collor must differ
                if (topCard.similarColorTo(card)) {
                    return false;
                }
            }
        } else {
            // Card stack is empty, first card must be king or turned up card.
            if (card.value() != 13 && card.isTurnedFaceUp()) {
                return false;
            }
        }

        // All checks are OK -> Add card on top.
        this.deck.addLast(card);
        return true;
    }
}
