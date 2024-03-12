create table server_info (
    name        text,
    description text
);

insert into server_info 
    (name, description)
  values
    ('Timesheets', 'Experimental timesheets server');

create rule dont_insert as
    on insert to server_info
    do nothing;

create rule dont_delete as
    on delete to server_info
    do nothing;


