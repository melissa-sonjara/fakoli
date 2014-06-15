-- Fakoli Login Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `login_token` (
  `token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(10) NOT NULL,
  `expire_date` date NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`token_id`),
  UNIQUE KEY `token_idx` (`token`)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

CREATE TABLE login_attempt
(
	login_attempt_id	int(10) not null auto_increment,
	username			varchar(200) not null default '',
	ip_address			varchar(50) not null,
	date_received		datetime not null,
	login_mode			varchar(20) not null,
	login_token			varchar(200) not null default '',
	result				varchar(20) not null,
	PRIMARY KEY(login_attempt_id)
) ENGINE=InnoDB;

-- END Version 1.1

-- START Version 1.2

CREATE TABLE login_account_lock
(
	lock_id			int(10) not null auto_increment,
	username		varchar(200) not null,
	lock_date		datetime not null,
	locked_until	datetime not null,
	active			tinyint(3) not null default 1,
	PRIMARY KEY(lock_id)
) ENGINE=InnoDB;

-- END Version 1.2