(define (problem mapf)
    (:domain graph-with-agents)

    (:objects
        rur, lul - agent
        a, c, e, f, d, b - vertex
    )

    (:init
        (is-at rur a)
        (occupied a)
        (is-at lul d)
        (occupied d)

        (edge b d)
        (edge d b)
        (edge e d)
        (edge d e)
        (edge d c)
        (edge c d)
        (edge a b)
        (edge b a)
        (edge b c)
        (edge c b)
        (edge e f)
        (edge f e)
    )

    (:goal ( and
        (is-at rur f)
        (is-at lul b)
    ))
)
