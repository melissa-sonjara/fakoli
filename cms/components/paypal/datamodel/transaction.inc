<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/



/*
 * 
 * 
 * The class that holds the paypal tranaction or order data may be
 * called "Transaction" or "Order" or some other name. This file shows
 * a sandbox class that is an examle of the fields you may wish to 
 * include in your transaction/order class.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 5/3/2011
 * 
 */



class PayPalSandboxTransaction extends DataItem
{
	var $fields = array(
					"transaction_id"	=>	Number,
					"user_id"			=>	Number,
					"amount"			=>	Currency, // cost of purchase
					"order_date"		=>	DateTime,
					"payment_transaction_id"	=>	String, // provided by PayPal
					"payment_amount"	=>	Currency, // amount PayPal reports was actually paid
					"payment_type"		=>	String,
					"status"			=>	String,
	);
	
	var $relations = array(
					"User"						=>	""
	);
		
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	function PayPalSandboxTransaction()
	{
		$this->table = "paypal_sandbox_transaction";
		$this->primary_key = "transaction_id";
		
		$this->DataItem(func_get_args());
	}
}

