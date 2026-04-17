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

/**
 * FormValidator — CSS-class-based form validation with inline error messages.
 *
 * ES equivalent of MooTools More Form.Validator + Form.Validator.Inline
 * + Form.Validator.Extras + InputValidator.
 *
 * Depends on locale.js for error message strings, and optionally on
 * date.js for date-related validators.
 *
 * Usage:
 *   var fv = new FormValidator(document.querySelector('form'));
 *   fv.validate();
 *
 * Validators are registered via:
 *   FormValidator.add('my-validator', { errorMsg: '...', test: fn });
 *   FormValidator.addAllThese([['v1', {...}], ['v2', {...}]]);
 *
 * Fields declare validators via CSS classes, e.g.:
 *   <input class="required validate-email" type="email" name="email">
 *
 * Props may be embedded as JSON in data-validator-properties attribute:
 *   <input class="minLength" data-validator-properties='{"minLength":5}'>
 */

// ── InputValidator ────────────────────────────────────────────────────────

class InputValidator
{
	/**
	 * @param {string} className   The CSS class that triggers this validator.
	 * @param {Object} options
	 * @param {string|Function|false} options.errorMsg  Error message or factory fn(field, props).
	 * @param {Function} options.test  fn(field, props) → boolean.
	 */
	constructor(className, options)
	{
		this.className = className;
		this.options   = Object.assign(
			{
				errorMsg: 'Validation failed.',
				test:     function() { return true; }
			},
			options || {}
		);
	}

	/**
	 * Test the field.
	 * @param {HTMLElement} field
	 * @param {Object}      [props]
	 * @returns {boolean}
	 */
	test(field, props)
	{
		return field ? this.options.test(field, props || this._getProps(field)) : false;
	}

	/**
	 * Get the error message for a field.
	 * @param {HTMLElement} field
	 * @param {Object}      [props]
	 * @returns {string}
	 */
	getError(field, props)
	{
		var err = this.options.errorMsg;
		if (typeof err === 'function') err = err(field, props || this._getProps(field));
		return err || '';
	}

	/** @private Parse validator props from field attributes/classes. */
	_getProps(field)
	{
		if (!field) return {};
		if (field._validatorProps) return field._validatorProps;

		// Try data-validator-properties attribute
		var attrStr = field.getAttribute('data-validator-properties') || field.getAttribute('validatorProps');
		if (attrStr)
		{
			try { return field._validatorProps = JSON.parse(attrStr); }
			catch(e) { return {}; }
		}

		// Parse from class names like "minLength:5"
		var classes  = Array.from(field.classList);
		var props    = {};
		classes.forEach(function(cls)
		{
			var colon = cls.indexOf(':');
			if (colon < 0) return;
			var key = cls.substring(0, colon);
			try { props[key] = JSON.parse(cls.substring(colon + 1)); }
			catch(e) {}
		});
		return field._validatorProps = props;
	}
}

// ── FormValidator ─────────────────────────────────────────────────────────

class FormValidator
{
	/**
	 * @param {HTMLFormElement} form
	 * @param {Object} [options]
	 * @param {string}   options.fieldSelectors           Selector for fields to validate (default standard).
	 * @param {boolean}  options.ignoreHidden             Skip hidden fields (default true).
	 * @param {boolean}  options.ignoreDisabled           Skip disabled fields (default true).
	 * @param {boolean}  options.useTitles                Use field title attr for error (default false).
	 * @param {boolean}  options.evaluateOnSubmit         Validate on submit (default true).
	 * @param {boolean}  options.evaluateFieldsOnBlur     Validate on blur (default true).
	 * @param {boolean}  options.evaluateFieldsOnChange   Validate on change (default true).
	 * @param {boolean}  options.serial                   Stop after first failed field (default true).
	 * @param {boolean}  options.stopOnFailure            Prevent submit on failure (default true).
	 * @param {boolean}  options.scrollToErrorsOnSubmit   Scroll to first error on submit (default true).
	 * @param {string}   options.errorPrefix              Prefix for error messages (default 'Error: ').
	 * @param {string}   options.warningPrefix            Prefix for warnings (default 'Warning: ').
	 * @param {Function} options.onFormValidate           Called with (isValid, form, event).
	 * @param {Function} options.onElementPass            Called with (field).
	 * @param {Function} options.onElementFail            Called with (field, validatorsFailed).
	 */
	constructor(form, options)
	{
		this.element = form;
		this.options = Object.assign(
			{
				fieldSelectors:           'input, select, textarea',
				ignoreHidden:             true,
				ignoreDisabled:           true,
				useTitles:                false,
				evaluateOnSubmit:         true,
				evaluateFieldsOnBlur:     true,
				evaluateFieldsOnChange:   true,
				serial:                   true,
				stopOnFailure:            true,
				scrollToErrorsOnSubmit:   true,
				errorPrefix:              null,
				warningPrefix:            null,
				onFormValidate:           null,
				onElementPass:            null,
				onElementFail:            null
			},
			options || {}
		);

		this._paused    = false;
		this._timer     = null;
		this._onSubmit  = this._handleSubmit.bind(this);
		this._onBlurChange = this._handleBlurChange.bind(this);

		this.warningPrefix = this.options.warningPrefix ||
			FormValidator.getMsg('warningPrefix') || 'Warning: ';
		this.errorPrefix   = this.options.errorPrefix   ||
			FormValidator.getMsg('errorPrefix')   || 'Error: ';

		this.enable();
		form._validator = this;
	}

	/** @returns {HTMLElement[]} All validatable fields in the form. */
	getFields()
	{
		return Array.from(this.element.querySelectorAll(this.options.fieldSelectors));
	}

	/** Attach event listeners. */
	enable()
	{
		if (this.options.evaluateOnSubmit)
		{
			this.element.addEventListener('submit', this._onSubmit);
		}
		if (this.options.evaluateFieldsOnBlur || this.options.evaluateFieldsOnChange)
		{
			this.element.addEventListener('blur',   this._onBlurChange, true);
			this.element.addEventListener('change', this._onBlurChange, true);
		}
		return this;
	}

	/** Detach event listeners. */
	disable()
	{
		this.element.removeEventListener('submit', this._onSubmit);
		this.element.removeEventListener('blur',   this._onBlurChange, true);
		this.element.removeEventListener('change', this._onBlurChange, true);
		return this;
	}

	/** Pause validation. */
	stop()
	{
		this._paused = true;
		return this;
	}

	/** Resume validation. */
	start()
	{
		this._paused = false;
		return this;
	}

	/** Mark a field as ignored (excluded from validation). */
	ignoreField(field, warnOnly)
	{
		field.classList.remove('ignoreValidation', 'warnOnly');
		field.classList.add(warnOnly ? 'warnOnly' : 'ignoreValidation');
		return this;
	}

	/** Remove ignored/warnOnly state from a field. */
	enforceField(field)
	{
		field.classList.remove('ignoreValidation', 'warnOnly');
		return this;
	}

	/** Validate all fields. Returns true if all valid. */
	validate(event)
	{
		var self   = this;
		var fields = this.getFields();
		var result = fields.map(function(field) { return self.validateField(field, true); })
			.every(function(v) { return v; });

		if (this.options.onFormValidate) this.options.onFormValidate.call(this, result, this.element, event);
		if (this.options.stopOnFailure && !result && event) event.preventDefault();

		if (!result && this.options.scrollToErrorsOnSubmit)
		{
			var failed = this.element.querySelector('.validation-failed');
			if (failed) failed.scrollIntoView({ behavior: 'smooth', block: 'center' });
		}

		return result;
	}

	/**
	 * Validate a single field.
	 * @param {HTMLElement} field
	 * @param {boolean}     [force]
	 * @returns {boolean}
	 */
	validateField(field, force)
	{
		if (this._paused) return true;

		var opts          = this.options;
		var cs            = window.getComputedStyle(field);
		if (opts.ignoreHidden   && cs.display === 'none')    return true;
		if (opts.ignoreDisabled && field.disabled)            return true;

		// Serial mode: skip if another field already failed
		if (opts.serial && !force)
		{
			var existingFail = this.element.querySelector('.validation-failed');
			if (existingFail && existingFail !== field) return true;
		}

		var validatorNames   = this._getValidatorNames(field);
		var validatorsFailed = [];
		var warnOnly         = field.classList.contains('warnOnly');
		var self             = this;

		validatorNames.forEach(function(name)
		{
			if (!name) return;
			var warn = warnOnly || /^warn-/.test(name);
			var cn   = name.replace(/^warn-/, '');
			var iv   = self.getValidator(cn);
			if (!iv) return;

			var props   = iv._getProps(field);
			var isValid = field.classList.contains('ignoreValidation') || iv.test(field, props);

			self._handleAdvice(cn, field, iv, isValid, warn, props);

			if (!isValid && !warn) validatorsFailed.push(cn);
		});

		var passed = validatorsFailed.length === 0;

		if (!warnOnly && validatorNames.some(function(n) { return self.getValidator(n.replace(/^warn-/, '')); }))
		{
			if (passed)
			{
				field.classList.add('validation-passed');
				field.classList.remove('validation-failed');
				if (opts.onElementPass) opts.onElementPass.call(this, field);
			}
			else
			{
				field.classList.add('validation-failed');
				field.classList.remove('validation-passed');
				if (opts.onElementFail) opts.onElementFail.call(this, field, validatorsFailed);
			}
		}

		return passed;
	}

	/** Reset all fields' validation state. */
	reset()
	{
		var self = this;
		this.getFields().forEach(function(field) { self.resetField(field); });
		return this;
	}

	/** Reset a single field's validation state and hide its advice. */
	resetField(field)
	{
		field.classList.remove('validation-failed', 'validation-passed', 'warning');
		this._getValidatorNames(field).forEach(function(name)
		{
			var cn     = name.replace(/^warn-/, '');
			var advice = field._validatorAdvice && field._validatorAdvice[cn];
			if (advice) advice.style.display = 'none';
		});
		return this;
	}

	/** @private Show or hide advice for one validator on one field. */
	_handleAdvice(cn, field, iv, isValid, warn, props)
	{
		if (!field._validatorAdvice) field._validatorAdvice = {};

		if (!isValid)
		{
			var errMsg = (warn ? this.warningPrefix : this.errorPrefix) +
				(this.options.useTitles ? (field.title || iv.getError(field, props)) : iv.getError(field, props));

			var advice = field._validatorAdvice[cn];
			if (!advice)
			{
				advice = document.createElement('div');
				var fieldId    = field.id || (field.id = 'input_' + (field.name || Math.random().toString(36).slice(2)));
				advice.id      = 'advice-' + cn + '-' + fieldId;
				advice.style.display = 'none';
				this._insertAdvice(advice, field);
				field._validatorAdvice[cn] = advice;
			}

			advice.className = warn ? 'warning-advice' : 'validation-advice';
			advice.innerHTML  = errMsg;

			if (warn) field.classList.add('warning');

			if (advice.style.display === 'none') advice.style.display = 'block';
		}
		else
		{
			var existing = field._validatorAdvice && field._validatorAdvice[cn];
			if (existing) existing.style.display = 'none';
		}
	}

	/** @private Insert advice element after the field. */
	_insertAdvice(advice, field)
	{
		var props   = this._getFieldProps(field);
		var msgPos  = props && props.msgPos ? document.getElementById(props.msgPos) : null;

		if (msgPos)
		{
			msgPos.appendChild(advice);
		}
		else if (field.type && field.type.toLowerCase() === 'radio')
		{
			field.parentNode.appendChild(advice);
		}
		else
		{
			field.parentNode.insertBefore(advice, field.nextSibling);
		}
	}

	/** @private Parse stored field props (from data attribute). */
	_getFieldProps(field)
	{
		var attrStr = field.getAttribute('data-validator-properties') || field.getAttribute('validatorProps');
		if (attrStr)
		{
			try { return JSON.parse(attrStr); } catch(e) {}
		}
		return {};
	}

	/** @private Get validator class names declared on a field. */
	_getValidatorNames(field)
	{
		var names = (field.getAttribute('data-validators') || field.className || '')
			.split(/\s+/)
			.filter(function(n) { return n && n.indexOf(':') < 0; });
		// Also include key-value class names like "minLength:5" (the key part)
		return names;
	}

	/**
	 * Look up a registered validator by name.
	 * @param {string} name
	 * @returns {InputValidator|null}
	 */
	getValidator(name)
	{
		var cn = (name || '').split(':')[0];
		return FormValidator.validators[cn] || null;
	}

	// ── Event handlers ─────────────────────────────────────────────────────

	_handleSubmit(event)
	{
		this.validate(event);
	}

	_handleBlurChange(event)
	{
		var field = event.target;
		if (!field.matches(this.options.fieldSelectors)) return;
		var self = this;
		clearTimeout(this._timer);
		this._timer = setTimeout(function() { self.validateField(field, false); }, 50);
	}
}

// ── Static registry ────────────────────────────────────────────────────────

FormValidator.validators = {};

FormValidator.getMsg = function(key)
{
	return (typeof Locale !== 'undefined') ? Locale.get('FormValidator.' + key) : '';
};

FormValidator.add = function(className, options)
{
	FormValidator.validators[className] = new InputValidator(className, options);
};

FormValidator.addAllThese = function(list)
{
	list.forEach(function(item) { FormValidator.add(item[0], item[1]); });
};

// ── Built-in validators ────────────────────────────────────────────────────

FormValidator.add('IsEmpty',
{
	errorMsg: false,
	test: function(field)
	{
		if (field.type === 'select-one' || field.tagName.toLowerCase() === 'select')
		{
			return !(field.selectedIndex >= 0 && field.options[field.selectedIndex] && field.options[field.selectedIndex].value !== '');
		}
		return field.value == null || field.value.length === 0;
	}
});

FormValidator.addAllThese([

	['required',
	{
		errorMsg: function() { return FormValidator.getMsg('required'); },
		test: function(field) { return !FormValidator.validators['IsEmpty'].test(field); }
	}],

	['minLength',
	{
		errorMsg: function(field, props)
		{
			return props.minLength != null
				? (FormValidator.getMsg('minLength') || 'Please enter at least {minLength} characters (you entered {length} characters).')
					.replace('{minLength}', props.minLength)
					.replace('{length}', field.value.length)
				: '';
		},
		test: function(field, props)
		{
			return props.minLength == null || field.value.length >= (props.minLength || 0);
		}
	}],

	['maxLength',
	{
		errorMsg: function(field, props)
		{
			return props.maxLength != null
				? (FormValidator.getMsg('maxLength') || 'Please enter no more than {maxLength} characters (you entered {length} characters).')
					.replace('{maxLength}', props.maxLength)
					.replace('{length}', field.value.length)
				: '';
		},
		test: function(field, props)
		{
			return field.value.length <= (props.maxLength || 10000);
		}
	}],

	['validate-integer',
	{
		errorMsg: function() { return FormValidator.getMsg('integer'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) || /^(-?[1-9]\d*|0)$/.test(field.value);
		}
	}],

	['validate-numeric',
	{
		errorMsg: function() { return FormValidator.getMsg('numeric'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) ||
				/^-?(?:0$0(?=\d*\.)|[1-9]|0)\d*(\.\d+)?$/.test(field.value);
		}
	}],

	['validate-digits',
	{
		errorMsg: function() { return FormValidator.getMsg('digits'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) || /^[\d() .:\-\+#]+$/.test(field.value);
		}
	}],

	['validate-alpha',
	{
		errorMsg: function() { return FormValidator.getMsg('alpha'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) || /^[a-zA-Z]+$/.test(field.value);
		}
	}],

	['validate-alphanum',
	{
		errorMsg: function() { return FormValidator.getMsg('alphanum'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) || !/\W/.test(field.value);
		}
	}],

	['validate-date',
	{
		errorMsg: function(field, props)
		{
			var fmt = props.dateFormat || '%m/%d/%Y';
			if (typeof Date.prototype.format === 'function')
			{
				return (FormValidator.getMsg('dateSuchAs') || 'Please enter a valid date such as {date}')
					.replace('{date}', new Date().format(fmt));
			}
			return FormValidator.getMsg('dateInFormatMDY') || 'Please enter a valid date such as MM/DD/YYYY (i.e. "12/31/1999")';
		},
		test: function(field)
		{
			if (FormValidator.validators['IsEmpty'].test(field)) return true;
			var date = Date.parse ? Date.parse(field.value) : new Date(field.value);
			return date && !isNaN(date);
		}
	}],

	['validate-email',
	{
		errorMsg: function() { return FormValidator.getMsg('email'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) ||
				/^(?:[a-z0-9!#$%&'*+\/=?^_`{|}~-]\.?){0,63}[a-z0-9!#$%&'*+\/=?^_`{|}~-]@(?:(?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\.)*[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\])$/i.test(field.value);
		}
	}],

	['validate-url',
	{
		errorMsg: function() { return FormValidator.getMsg('url'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) ||
				/^(https?|ftp|rmtp|mms):\/\/(([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:(\d+))?\/?/i.test(field.value);
		}
	}],

	['validate-currency-dollar',
	{
		errorMsg: function() { return FormValidator.getMsg('currencyDollar'); },
		test: function(field)
		{
			return FormValidator.validators['IsEmpty'].test(field) ||
				/^\$?\-?([1-9]{1}[0-9]{0,2}(\,[0-9]{3})*(\.[0-9]{0,2})?|[1-9]{1}\d*(\.[0-9]{0,2})?|0(\.[0-9]{0,2})?|(\.[0-9]{1,2})?)$/.test(field.value);
		}
	}],

	['validate-one-required',
	{
		errorMsg: function() { return FormValidator.getMsg('oneRequired'); },
		test: function(field, props)
		{
			var container = props['validate-one-required']
				? (document.getElementById(props['validate-one-required']) || field.closest(props['validate-one-required']))
				: field.parentNode;
			return container
				? Array.from(container.querySelectorAll('input')).some(function(el)
				{
					if (el.type === 'checkbox' || el.type === 'radio') return el.checked;
					return !!el.value;
				})
				: false;
		}
	}],

	// ── Extras ───────────────────────────────────────────────────────────

	['validate-nospace',
	{
		errorMsg: function() { return FormValidator.getMsg('noSpace'); },
		test: function(field) { return !/\s/.test(field.value); }
	}],

	['validate-required-check',
	{
		errorMsg: function(field, props)
		{
			return props.useTitle ? (field.title || '') : FormValidator.getMsg('requiredChk');
		},
		test: function(field) { return !!field.checked; }
	}],

	['validate-match',
	{
		errorMsg: function(field, props)
		{
			var matchEl   = props.matchInput ? document.getElementById(props.matchInput) : null;
			var matchName = props.matchName || (matchEl ? matchEl.name : '');
			return (FormValidator.getMsg('match') || 'This field needs to match the {matchName} field')
				.replace('{matchName}', matchName);
		},
		test: function(field, props)
		{
			var matchEl = props.matchInput ? document.getElementById(props.matchInput) : null;
			if (!matchEl) return true;
			return !field.value || field.value === matchEl.value;
		}
	}],

	['validate-after-date',
	{
		errorMsg: function(field, props)
		{
			var label = props.afterLabel ||
				(props.afterElement ? FormValidator.getMsg('startDate') : FormValidator.getMsg('currentDate'));
			return (FormValidator.getMsg('afterDate') || 'The date should be the same or after {label}.')
				.replace('{label}', label);
		},
		test: function(field, props)
		{
			var startEl = props.afterElement ? document.getElementById(props.afterElement) : null;
			var start   = startEl ? Date.parse(startEl.value) : new Date();
			var end     = Date.parse(field.value);
			return end && start ? end >= start : true;
		}
	}],

	['validate-before-date',
	{
		errorMsg: function(field, props)
		{
			var label = props.beforeLabel ||
				(props.beforeElement ? FormValidator.getMsg('endDate') : FormValidator.getMsg('currentDate'));
			return (FormValidator.getMsg('beforeDate') || 'The date should be the same or before {label}.')
				.replace('{label}', label);
		},
		test: function(field, props)
		{
			var endEl = props.beforeElement ? document.getElementById(props.beforeElement) : null;
			var start = Date.parse(field.value);
			var end   = endEl ? Date.parse(endEl.value) : new Date();
			return start && end ? end >= start : true;
		}
	}],

	['validate-same-month',
	{
		errorMsg: function(field, props)
		{
			var sameMoEl  = props.sameMonthAs ? document.getElementById(props.sameMonthAs) : null;
			var startMoVal = sameMoEl ? sameMoEl.value : '';
			return FormValidator.getMsg(startMoVal ? 'sameMonth' : 'startMonth') || '';
		},
		test: function(field, props)
		{
			var sameMoEl = props.sameMonthAs ? document.getElementById(props.sameMonthAs) : null;
			var d1 = Date.parse(field.value);
			var d2 = sameMoEl ? Date.parse(sameMoEl.value) : null;
			if (!d1 || !d2) return true;
			return d1.getMonth() === d2.getMonth() && d1.getFullYear() === d2.getFullYear();
		}
	}],

	['validate-cc-num',
	{
		errorMsg: function(field)
		{
			var ccNum = field.value.replace(/[^0-9]/g, '');
			return (FormValidator.getMsg('creditcard') || 'The credit card number entered is invalid. {length} digits entered.')
				.replace('{length}', ccNum.length);
		},
		test: function(field)
		{
			if (FormValidator.validators['IsEmpty'].test(field)) return true;
			var ccNum = field.value.replace(/[^0-9]/g, '');

			var valid = /^4[0-9]{12}([0-9]{3})?$/.test(ccNum) ||
				/^5[1-5][0-9]{14}$/.test(ccNum) ||
				/^3[47][0-9]{13}$/.test(ccNum) ||
				/^6(?:011|5[0-9]{2})[0-9]{12}$/.test(ccNum) ||
				/^3(?:0[0-5]|[68][0-9])[0-9]{11}$/.test(ccNum);

			if (valid)
			{
				// Luhn check
				var sum = 0;
				for (var i = ccNum.length - 1; i >= 0; i--)
				{
					var cur = parseInt(ccNum.charAt(i), 10);
					if ((ccNum.length - i) % 2 === 0) cur *= 2;
					if (cur > 9) cur -= 9;
					sum += cur;
				}
				return sum % 10 === 0;
			}
			return false;
		}
	}],

	['validate-enforce-oncheck',
	{
		test: function(field, props)
		{
			var form = field.closest('form');
			var fv   = form && form._validator;
			if (!fv) return true;
			var sel = props.toEnforce || 'input, select, textarea';
			var container = props.enforceChildrenOf ? document.getElementById(props.enforceChildrenOf) : null;
			var targets = container
				? Array.from(container.querySelectorAll(sel))
				: (typeof sel === 'string' ? [] : Array.from(sel));
			targets.forEach(function(item)
			{
				if (field.checked) fv.enforceField(item);
				else { fv.ignoreField(item); fv.resetField(item); }
			});
			return true;
		}
	}],

	['validate-ignore-oncheck',
	{
		test: function(field, props)
		{
			var form = field.closest('form');
			var fv   = form && form._validator;
			if (!fv) return true;
			var container = props.ignoreChildrenOf ? document.getElementById(props.ignoreChildrenOf) : null;
			var sel     = 'input, select, textarea';
			var targets = container ? Array.from(container.querySelectorAll(sel)) : [];
			targets.forEach(function(item)
			{
				if (field.checked) { fv.ignoreField(item); fv.resetField(item); }
				else                fv.enforceField(item);
			});
			return true;
		}
	}],

	['validate-reqchk-bynode',
	{
		errorMsg: function() { return FormValidator.getMsg('reqChkByNode'); },
		test: function(field, props)
		{
			var container = props.nodeId ? document.getElementById(props.nodeId) : field.parentNode;
			var sel = props.selector || 'input[type=checkbox], input[type=radio]';
			return container
				? Array.from(container.querySelectorAll(sel)).some(function(el) { return el.checked; })
				: false;
		}
	}],

	['validate-reqchk-byname',
	{
		errorMsg: function(field, props)
		{
			return (FormValidator.getMsg('reqChkByName') || 'Please select a {label}.')
				.replace('{label}', props.label || field.type);
		},
		test: function(field, props)
		{
			var name = props.groupName || field.name;
			var els  = Array.from(document.querySelectorAll('[name="' + name + '"]'));
			var ok   = els.some(function(el) { return el.checked; });
			if (ok)
			{
				var form = field.closest('form');
				var fv   = form && form._validator;
				if (fv) els.forEach(function(el) { fv.resetField(el); });
			}
			return ok;
		}
	}],

	['validate-custom-required',
	{
		errorMsg: function() { return FormValidator.getMsg('required'); },
		test: function(field, props)
		{
			return field.value !== props.emptyValue;
		}
	}]

]);

// ── Element.prototype helper ──────────────────────────────────────────────

/**
 * Validate this form element, optionally applying options first.
 * @param {Object} [options]
 * @returns {boolean}
 */
Element.prototype.validate = function(options)
{
	if (!this._validator) new FormValidator(this, options);
	else if (options) Object.assign(this._validator.options, options);
	return this._validator.validate();
};
