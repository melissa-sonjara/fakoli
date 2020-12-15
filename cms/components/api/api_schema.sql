-- Fakoli API Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `api_token` (
	`token_id`		int(10) not null auto_increment,
	`token`			varchar(200) not null,
	`user_id`		int(10) not null,
	`created_date`	datetime not null,
	`expiry_date`	datetime null,
	`last_access`	datetime null,
	`active`		tinyint(3) not null default 0,
	PRIMARY KEY(token_id)
);

-- END Version 1.0