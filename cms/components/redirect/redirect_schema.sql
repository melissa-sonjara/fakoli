-- Component Schema for redirect
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE redirect
(
	redirect_id		int(10) not null auto_increment,
	redirect_from 	varchar(1000) not null,
	redirect_to 	varchar(1000) not null,
	user_id			int(10) not null,
	active			tinyint(3) not null default 0,
	last_modified	datetime not null,
	PRIMARY KEY(redirect_id)
) ENGINE=InnoDB;

-- END Version 1.0
