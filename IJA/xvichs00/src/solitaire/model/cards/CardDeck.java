package solitaire.model.cards;

import java.util.*;
import java.io.Serializable;

/**
 * Class represents card deck.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class CardDeck implements Serializable {

    /**
     * List of cards.
     */
    protected LinkedList<Card> deck = new LinkedList<>();

    /**
     * Constructor.
     */
    public CardDeck() {

    }

    /**
     * Appends the specified card to the end of this list.
     *
     * @param card Specified card.
     * @return True.
     */
    public boolean put(Card card) {
        this.deck.addLast(card);
        return true;
    }

    /**
     * Appends the specified deck to the end of this list.
     *
     * @param newDeck Deck to append.
     * @return True if operation was succesful.
     */
    public boolean put(CardDeck newDeck) {
        CardDeck s = (CardDeck) newDeck;
        for (int i = 0; i < s.size(); i++) {
            if (this.put(s.getCard(i)) == false) {
                return false;
            }
        }
        return true;
    }

    /**
     * Turns all cards down.
     */
    public void turnDownAll() {
        for (int i = 0; i < deck.size(); i++) {
            getCard(i).turnFaceDown();
        }
    }

    /**
     * Takes off card from top of the stack. If deck is empty, returns null.
     *
     * @return Card from top of the stack.
     */
    public Card pop() {
        Card card;
        try {
            card = this.deck.removeLast();
        } catch (Exception e) {
            return null;
        }
        return card;
    }

    /**
     * Takes off stack of cards which are upwards from entered card.
     *
     * @param card First card to take off from stack.
     * @return New stack of cards which are upwards from entered card.
     */
    public CardDeck pop(Card card) {
        int i = this.deck.indexOf(card);
        CardDeck newDeck = new CardDeck();
        // Copies cards to new deck.
        for (int recursive_index = i; recursive_index < this.size(); recursive_index++) {
            newDeck.put(this.getCard(recursive_index));
        }
        // Deletes cards from original deck
        int packSize = this.size();
        for (int recursive_index = i; recursive_index < packSize; recursive_index++) {
            this.pop();
        }
        return newDeck;
    }

    /**
     * Takes off specific number of cards from top of the stack
     *
     * @param length Number of cards.
     * @return New stack of cards.
     */
    public CardDeck pop(int length) {
        if (length <= 0) {
            return null;
        }
        int i = size() - length;
        if (i < 0) {
            i = 0;
        }
        CardDeck newDeck = new CardDeck();
        for (int recursive_index = i; recursive_index < this.size(); recursive_index++) {
            newDeck.put(this.getCard(recursive_index));
        }                                                           // -------------------------------- deleted ;
        int packSize = this.size();
        for (int recursive_index = i; recursive_index < packSize; recursive_index++) {
            this.pop();
        }
        return newDeck;
    }

    /**
     * Gets card from stack by given index. Card stays in stack.
     *
     * @param index Index.
     * @return Card from stack.
     */
    public Card get(int index) {
        // Index check
        if (index < 0 || index > this.size() - 1) {
            return null;
        }
        return this.deck.get(index);
    }

    /**
     * Gets card from stack by given index. Card stays in stack.
     *
     * @param i Index
     * @return Card from stack.
     */
    public Card getCard(int i) {
        return this.get(i);
    }

    /**
     * Gets card from top of the stack. Card stays in stack.
     *
     * @return Card from top of the stack.
     */
    public Card get() {
        return this.get(this.size() - 1);
    }

    /**
     * Shuffles deck.
     */
    public void shuffle() {
        Collections.shuffle(this.deck);
    }

    /**
     * Check if given object is equals to this.
     *
     * @param o Given object to check.
     * @return True if objects are same.
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null) {
            return false;
        }
        if (getClass() == o.getClass()) {
            // Objects are same if cards equals eachother.
            // Checks number of cards in objects.
            CardDeck s = (CardDeck) o;
            if (this.size() != s.size()) {
                return false;
            }
            for (int i = 0; i < s.size(); i++) {
                if (!this.getCard(i).equals(s.getCard(i))) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }

    }

    @Override
    public int hashCode() {         // -------------------------------- HashCode
        int hashCode = 0;
        for (int i = 0; i < this.size(); i++) {
            hashCode += this.deck.get(i).hashCode();
        }
        return hashCode;
    }

    /**
     * Size of deck.
     *
     * @return Size of deck.
     */
    public int size() {
        return this.deck.size();
    }

    /**
     * Cheks if deck is empty.
     *
     * @return True if deck is empty.
     */
    public boolean isEmpty() {
        return this.deck.size() == 0;   // -------------------------------- this.deck.isEmpty()
    }

    @Override
    public String toString() {
        String returnString = "";
        for (int i = 0; i < this.size(); i++) {
            returnString += "[" + getCard(i).toString() + "]";
            if (i < size() - 1) {
                returnString += ", ";
            }
        }
        return returnString;
    }

}
