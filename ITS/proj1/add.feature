Feature: Adding a new product to a database

	Background: Admin is logged in the OpenCart System

	Scenario: Show Add Product page
		Given the admin is in the Product section
		When he clicks on the Add New button
		Then the Add Product page will appear
		
	Scenario: Adding a simple product
		Given the admin is in the Add Product page 
		And types <name> into the Product Name input
		And types required Meta Tag Title
		And types Model name
		And select category <cat> 
		When admin clicks on the Save button
		Then the product <name> is add to OpenCart system 
		And is visible in the online shop in selected category <cat>

	Scenario: Abort adding a new product
		Given the admin is in the Add Product page
		When he clicks on the Cancel button
		Then he is retured to Product page
		And no changes were save


