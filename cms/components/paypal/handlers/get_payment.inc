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
 * Build parameter string to request payment from PayPal.
 * This parameter set is for a non-hosted button, where
 * all the information about cost of products or services
 * is managed by our site and not stored on PayPal. We do
 * not provide PayPal with true quantities of items purchased
 * because that would require allowing them to manage
 * price, product offerings, and total cost calculations.
 * 
 * Note: for nonhosted buttons, it is not necessary to
 * create a button on the merchant's PayPal account.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 5/5/2011
 */

Fakoli::using("paypal");

$transaction_id = checkNumeric($_GET["transaction_id"]);
	
$transaction = new PayPalSandboxTransaction($transaction_id);

$merchant_id = Settings::getValue("paypal", "merchant_id");
$paypal_url = Settings::getValue("paypal", "paypal_url");

if(!$transaction_id || !$merchant_id || !$paypal_url)
	ajaxReturn("invalid parameters");

/*
 * These variables are broken out from the parameter string to
 * clearly identify the variables that need to be customized.
 * Other variables may need customization for specific implementations
 * (e.g., shipping charges or tax).
 * 
 * These variables are not in application settings b/c an application
 * may need more than one paypal checkout pathway. 
 */
$trans = "?transaction_id=$transaction_id";
$return = "http://". $config["http_host"] . "/payment_confirmation" . $trans;
$notify = "http://" . $config["http_host"] . "/action/paypal/transaction_handler" . $trans;
$cancel = "http://". $config["http_host"] . "/sandbox_purchase" . $trans;
$item_name = "Sandbox Item";
	
$param = "cmd=_xclick&business=$merchant_id";
$param .= "&lc=US";
$param .= "&item_name=$item_name";
$param .= "&item_number=1";
$param .= "&quantity=1";
$param .= "&shipping=0.00";
$param .= "&no_shipping=1";
$param .= "&tax=0.00";
$param .= "&button_subtype=services";
$param .= "&no_note=1";
$param .= "&currency_code=USD";
$param .= "&amount={$transaction->amount}";
$param .= "&rm=1";
$param .= "&return={$return}";
$param .= "&notify_url={$notify}";
$param .= "&cancel_return={$cancel}";
$param .= "&tax_rate=0.000";
$param .= "&bn=PP-BuyNowBF:btn_buynowCC_LG.gif:NonHosted";

redirect($paypal_url . "?$param");

?>
