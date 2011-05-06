<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

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
 * Handles Instance Payment Notification (IPN) from Paypal
 * If the user cancels payment, this handler is not called.
 * 
 * The application should alert admin/order fulfillment staff when/if
 * the paypal reported payment amount is not equal to the amount
 * expected: 
 * 
 * amount != payment_amount
 *  
 * This can occur if the user tampers with the parameter string in the
 * url at the PayPal site, changing the amount sent to Paypal. 
 * 
 * Janice Gallant for Sonjara, Inc.
 * 
 * 9/27/2010
 * 
 */

Fakoli::using("paypal", "email");

$transaction_id = checkNumeric($_GET["transaction_id"]);

if(!$transaction_id)
	redirect("/index");
	
$transaction = new PayPalSandboxTransaction($transaction_id);

// The timestamp is converted to datetime format
$date = date('Y-m-d H:m:s', strtotime($_POST["payment_date"]));
trace("TransactionHandler:: payment date $date", 3);

$transaction->filter = new InclusionFilter("order_date", "status", "payment_transaction_id", "payment_amount");
$transaction->order_date = $date;
$transaction->payment_transaction_id = $_POST["txn_id"];
$transaction->payment_amount = $_POST["mc_gross"];
$transaction->status = "submitted"; 
$transaction->save();

// Confirmation email must be created in CMS => Email Templates
sendEmailUsingEmailManager($transaction, 'paypal_sandbox_payment_confirmation');		

?>