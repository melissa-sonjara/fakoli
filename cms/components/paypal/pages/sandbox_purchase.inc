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

Fakoli::using("paypal");
Fakoli::usingFeature("auto_form");

if(!$user)
	redirect("/index");

$transaction = new PayPalSandboxTransaction();

$transaction->user_id = $user->user_id;
$transaction->payment_type = "PayPal";

$transaction->filter = new InclusionFilter("user_id", "payment_type", "amount");
$form = new AutoForm($transaction);

$form->hide("user_id", "payment_type");
$form->submitLabel = "Pay Now Using PayPal";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/action/paypal/get_payment?transaction_id={$form->data->transaction_id}");
	}
}

?>
<p>You can use the following "personal" sandbox account to "pay" for a sandbox/test PayPal purchase:</p>
<ul>
<li>user: janice_1285195089_per@sonjara.com</li>
<li>password: 285195014</li>
</ul>
<?php
$form->drawForm();
?>