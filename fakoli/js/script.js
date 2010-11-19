
function toggleContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	if (elt.style.display == 'block')
	{
		elt.style.display = 'none';
	}
	else
	{
		elt.style.display = 'block';
	}
}

function showContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'block';
}

function hideContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'none';
}
