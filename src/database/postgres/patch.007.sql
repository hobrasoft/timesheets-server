begin;

insert into version (version) values (7);

drop view if exists attendance.events_view;

create view attendance.events_view as
select
        e.event,
        e.date,
        e.event_type,
        t.description as event_description,
        e.employee,
        p.firstname,
        p.surname,
        e.valid,
        e.user_edited,
        u.name as user_edited_name
        
    from attendance.events e
    left join attendance.employees   p using (employee)
    left join users                  u on (e.user_edited = u."user")
    left join attendance.event_types t using (event_type)
    ;

commit;
