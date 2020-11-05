require 'test/unit'
require_relative "fib-sequence"

class FibonacciSequence < Test::Unit::TestCase
  # nastavi pocatecni cleny posloupnosti na 5 a 8
  def setup
    # nastavuje pomoci konstruktoru pocatecn cleny rady na 5 a nasledujici clen na 8 
  	@sequence = FibonacciSequence.new(5, 8)
  end

  # testuje zda funkce reset nenarusi ostatni procesy
  def test_reset
    @sequence.reset
    @sequence.next
    assert_equal(@sequence.current, 5)
  end

  # testuje zda funkce next spravne vraci jednotlive cleny ve spravnem poradi
  def test_next
  	@sequence.next
  	assert_equal(@sequence.current, 5)
  	@sequence.next
  	assert_equal(@sequence.current, 8)
    @sequence.next
    assert_equal(@sequence.current, 13)
  end

  # testuje zda funkce current spravne vraci honotu aktualniho indexu
  def test_current
  	@sequence.next
  	assert_equal(@sequence.current, 5)
    @sequence.next
    assert_equal(@sequence.current, 8)
    @sequence.reset
    assert_equal(@sequence.current, nil)
  end
  # testuje zda funkce current_idx vraci aktualni index
  def test_current_idx
  	@sequence.next
  	assert_equal(@sequence.current_idx, 0)
    @sequence.next
    assert_equal(@sequence.current_idx, 1)
  end

  # testuje spravnost volani navratovou hodnotu funkce[] (respektive pole)(v zavislosti na pocatecnim konstruktoru)
  def test_get_number_by_idx
    assert_equal(@sequence[0], 5)
  	assert_equal(@sequence[2], 13)
    assert_equal(@sequence[4], 34)
    assert_equal(@sequence[-33], nil)
  end

end