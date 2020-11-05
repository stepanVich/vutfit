Feature: Editing product

	Background:
		Given the admin in logged in the system
		And is on the Product page


	Scenario: Simple Product editing 
		Given the admin clicks on the Edit button of the product
		And rewrite the Description textarea
		When he clicks on the Save button
		Then the change is saved into the system
		And is visible in the Product page on online eshop


	Scenario: Complex Product editing 
		Given the admin clicks on the Edit button 
		And the admin rewrite the Product Name
		And Product Description
		And Price
		When he clicks on the Save button
		Then the change is saved into the system
		And the change is visible after reload on the same page in online eshop

	Scenario: Product remove 
		Given the admin selects multiple products in the list of products
		When he clicks on the Delete button
		Then all the selected products are removed from the system
		And can not be longer viewed in the online eshop

	Scenario: Product Copy
		Given the admin select one product in the list of products
		When he clicks on the Copy button
		Then the selected product is copiied in the list of products


 
