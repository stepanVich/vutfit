package solitaire.model.cards;

import java.io.Serializable;

/**
 * Class represents card.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class Card implements Serializable {

    /**
     * Cards value.
     */
    protected int value;
    /**
     * Cards color.
     */
    protected Color color;
    /**
     * If cards is turned up.
     */
    protected boolean isTurnUp;

    /**
     * Enum of card color.
     */
    public static enum Color {
        CLUBS, DIAMONDS, HEARTS, SPADES;

        public String toString() {
            switch (this) {
                // Kary  - cerne
                case CLUBS:
                    return "C";
                // Piky  - cervene
                case DIAMONDS:
                    return "D";
                // Srdce - cervene
                case HEARTS:
                    return "H";
                // Listy - cerne
                case SPADES:
                    return "S";
            }
            return "";
        }
    };

    /**
     * Card constructor.
     *
     * @param c Cards color.
     * @param value Cards value.
     */
    public Card(Card.Color c, int value) {
        this.value = value;
        this.color = c;
        // Each cards is created turned down.
        this.isTurnUp = false;
    }

    /**
     * Gets card color.
     *
     * @return Color of the card.
     */
    public Color color() {
        return this.color;
    }

    /**
     * Gets card value.
     *
     * @return Value of the card.
     */
    public int value() {
        return this.value;
    }

    /**
     * Checks if card is turned up.
     *
     * @return True if cards is turned up.
     */
    public boolean isTurnedFaceUp() {
        return this.isTurnUp;
    }

    /**
     * Turns card up.
     *
     * @return True if card was turned down. Returns false if card was turned
     * up.
     */
    public boolean turnFaceUp() {
        boolean returnValue = this.isTurnUp;
        this.isTurnUp = true;
        if (!(returnValue && this.isTurnUp)) {
            return true;
        }
        return false;
    }

    /**
     * Turns card down.
     *
     * @return True if card was turned down. Returns false if card was turned
     * up.
     */
    public boolean turnFaceDown() {
        boolean returnValue = this.isTurnUp;
        this.isTurnUp = false;
        if ((returnValue && !this.isTurnUp)) {
            return true;
        }
        return false;
    }

    /**
     * Compares cards value to this.
     *
     * @param c Card to compare.
     * @return Difference between cards.
     */
    public int compareValue(Card c) {
        return this.value - c.value();
    }

    /**
     * Check if cards color is similar to this.
     *
     * @param c Specified card.
     * @return True if cards are similar, false if not.
     */
    public boolean similarColorTo(Card c) {
        // Black color
        if ((this.color == Card.Color.SPADES || this.color == Card.Color.CLUBS) && (c.color() == Card.Color.SPADES || c.color() == Card.Color.CLUBS)) {
            return true;
        }
        // Red color
        if ((this.color == Card.Color.HEARTS || this.color == Card.Color.DIAMONDS) && (c.color() == Card.Color.HEARTS || c.color() == Card.Color.DIAMONDS)) {
            return true;
        }
        // Colors differ
        return false;
    }

    @Override
    public int hashCode() {
        return this.color.hashCode() + this.value;
    }

    @Override
    public boolean equals(Object o) {
        // Self check
        if (this == o) {
            return true;
        }
        // Null check
        if (o == null) {
            return false;
        }
        if (getClass() != o.getClass()) {
            return false;
        }
        Card card = (Card) o;
        return this.color == card.color() && this.value == card.value();
    }

    @Override
    public String toString() {
        String hodnotaKarty = Integer.toString(this.value);
        switch (this.value) {
            case 11:
                hodnotaKarty = "J";
                break;
            case 12:
                hodnotaKarty = "Q";
                break;
            case 13:
                hodnotaKarty = "K";
                break;
        }
        return hodnotaKarty + "(" + this.color.toString() + ")";
    }

}
