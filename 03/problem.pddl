(define (problem mapf)
        (:domain graph-with-agents)
        (:objects
                andy, rur - agent
                s, a, b, c, d, f - vertex
        )
        (:init
                (is-at andy s)
                (occupied s)

                (is-at rur c)
                (occupied c)

                (edge s a)
                (edge a c)
                (edge a b)
                (edge b d)
                (edge d f)
        )
        (:goal (and
                (is-at andy f)
                (is-at rur s)
        ))
)

