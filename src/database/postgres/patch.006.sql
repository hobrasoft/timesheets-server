alter table statuses add column show_in_overview boolean default false;
update statuses set show_in_overview = true;
