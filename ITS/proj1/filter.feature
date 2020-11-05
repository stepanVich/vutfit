Feature: Listing and filtering the products from database

	Background:
		Given the admin in logged in the system
		And is on the Product page


	Scenario: List all products 
		Given the filter is empty
		When the user clicks the Filter button
		Then all the products in the system are listed				

	Scenario: Filtering products by Product Name 
		Given the Product Name input in Filter Form  is set to <name>
		When the user clicks the Filter button
		Then all the products that contain <name> in Product Name are returned in list

	Scenario: Filtering products by Status
		Given the Status selection box in Filter Form is set to <status>
		When the user clicks the Filter button
		Then all the products that have Status = <status> are returned in list


	Scenario: Filtering products by Quantity
		Given the Quantity input in Filter Form is set to <quantity>
		When the user clicks the Filter button
		Then all the products that have Quantity = <quantity> are returned in list


	Scenario: Complex Filtering products by Product Name and Status
		Given the Product Name input in Filter Form is set to <name>
		And the Status selection box in Filter Form is set to <status>
		When the user clicks the Filter button
		Then all the products that contain the same <name> and <status> are returned in list 




