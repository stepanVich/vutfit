class Equation 
  def self.solve_quadratic(a, b, c)

  	# pokud je a nenulove proved slozity vypocet
	if a != 0
		# vypocitej diskriminant
		discriminant = b*b-4*a*c 
		# pokud je diskriminant nulovy vrat jeden vysledek
		if discriminant == 0
		  return [-b/2*a]
		# pokud je diskriminant mensi nez nula je vysledek prazdna mnozina
	    elsif discriminant < 0
	    	return nil
	    # pokud je diskriminant vetsi nez nula vrat dvojnasobny koren
	    elsif discriminant > 0
	        return [(-b+Math.sqrt(discriminant))/(2*a), (-b-Math.sqrt(discriminant))/(2*a)]
		end
	end


	# pokud je a nulove proved jednoduchy vypocet
	if a == 0
		# pokud je b i a nulove je vysledek nil
		if b == 0
			return nil
		end

		# pokud je b nenulove vrat jeden vysledek rovnice
		return [-c/b.to_f]
	end
  end
end