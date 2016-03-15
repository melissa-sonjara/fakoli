<h2>Scheduled Tasks</h2>
<p>Web applications sometimes have periodic housekeeping tasks that need to be run in
the background without user intervention, such as sending out email digests, reindexing
SOLR search indexes, or other maintenance activities. The Scheduled Tasks component provides
this functionality.</p>
<p>Your server will need some configuration to be able to run scheduled tasks. On a UNIX
style machine, this involves adding a worker script into crontab. The steps for setting up
such a script are as follows:</p>
<ol>
 <li>First you need to create a secure authentication token that can be used to log in
 your scheduled task worker. To do this, go to <b>User Manager > Authentication Tokens</b>
 and create a token with the <b>User</b> configured to a user with sufficient permissions to
 run the scheduled workers (usually <em>admin</em>), <b>Enabled</b> checked and <b>Redirect on Success</b>
 set to <tt>/action/scheduled_task/run_tasks</tt>. For extra security you can add the local IP
 address for you server to restrict the token to local access only. You can set the token name
 directly, or a secure randomized token name can be generated for you.</li>
 <li>First create the script and configure it to be run hourly. For most modern UNIX style
 systems this involves creating file in <tt>/etc/cron.hourly</tt>. A simple example script
 for the site <tt>my.fakoli.org</tt> is shown below (you would replace the <tt>my.fakoli.org</tt>
 hostname and the token with the values for your own server):
 <pre>
#!/bin/bash

wget -O /dev/null -a /srv/www/logs/my.fakoli.org.tasks.log http://my.fakoli.org/action/auto_login/authenticate?token=MYSECURETOKEN
 </pre>
 If your server is running under SSL (recommended) you need to use a slightly different script:
<pre>
#!/bin/bash

wget -O /dev/null --no-check-certificate -a /srv/www/logs/my.fakoli.org.tasks.log https://my.fakoli.org/action/auto_login/authenticate?token=MYSECURETOKEN
 </pre>
 </li>
 <li>Save the script and make it executable by running the command <tt>chmod a+x <i>{script name}</i></tt>
 </li>
</ol> 
<p><em>N.B. Depending on your server set up, you will need to be logged in as a root user or sudoed in order
to complete the above operations.</em></p>
<p>Once the script is in place it will be called once an hour at the time offset specified in your <tt>/etc/crontab</tt> file.
You can test the configuration by enabling the <b>Scheduled Task: Validate Configuration</b> task. This will send a verification
message to a chosen email address every time it is triggered. You can configure the email address this is sent to via the <b>Scheduled Tasks</b>
tab in <b>Site Configuration > Application Setting</b>.
</p>
