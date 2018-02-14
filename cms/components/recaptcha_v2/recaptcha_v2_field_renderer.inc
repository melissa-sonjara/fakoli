<?php
/**************************************************************

Copyright (c) 2018 Sonjara, Inc

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

Fakoli::usingFeature("field_renderers", "validation");
Fakoli::using("settings");


class RecaptchaV2Validator extends AbstractValidator
{
	var $message;
	var $compareField;		///< The field where the turing test id will be set in $_POST; controlled by hello captcha creator
	
	/**
	 * We do not receive parameters for this validator because the field
	 * names for retrieving values in $_POST is determined by the Recaptcha
	 * service provider.
	 */
	function __construct()
	{
		$this->message = "The code you entered did not match. Please try again.";
		$this->compareField = "recaptcha_response_field";
		$this->title = "Captcha";
		// Pass the field  name hello captcha creators give to the user input to compare against the test
		$this->AbstractValidator("recaptcha_user_answer", "Captcha");
	}
	
	function writeClient()
	{
		// No client-side component
		
		return "";
	}
	
	function validate()
	{
		if ($this->readOnly) return "";
		
		$secret_key = Settings::getValue("captcha", "recaptcha_v2_secret_key");
		
		if (!isset($_POST["g-recaptcha-response"]) || $_POST["g-recaptcha-response"] === $this->empty)
		{
			return "{$this->title} is a required field.";
		}
		
		$data = array(	"secret" 	=>	$secret_key,
						"response"	=>	$_POST["g-recaptcha-response"],
						"remoteip"	=>	$_SERVER["REMOTE_ADDR"]);
		$response = postRemote(RecaptchaV2Manager::$verificationURL, $data);
		$result = json_decode($response);
		if ($result->success == false)
		{
			return "CAPTCHA failed";
		}
		
		return "";
	}
}


class RecaptchaV2FieldRenderer extends FieldRenderer
{
	function __construct(&$form)
	{
		parent::__construct($form);
		
		$this->field = "captcha";
		$form->add($this);
		
		$site_key = Settings::getValue("captcha", "recaptcha_v2_site_key");
		$secret_key = Settings::getValue("captcha", "recaptcha_v2_secret_key");
		
		if(!$site_key || !$secret_key)
		{
			throw new FakoliException("reCAPTCHA v2 keys must be provided in CMS Application Settings.");
		}
		
		$this->hideLabel = true;
		$this->colspan = 2;
		$form->validator->add(new RecaptchaV2Validator());
	}
	
	function renderScript($field)
	{
?>
	<script src='https://www.google.com/recaptcha/api.js'></script>
<?
	}
	
	function renderField($field = "")
	{
		$site_key = Settings::getValue("captcha", "recaptcha_v2_site_key");
		
		if (!$field) $field = $this->field;
		
		$this->_startField($field);
?>
	<div class='captcha_wrapper'>
		<div class="g-recaptcha" data-sitekey="<?echo $site_key?>"></div>
	</div>
<?
		$this->_endField($field);
	}
}

?>