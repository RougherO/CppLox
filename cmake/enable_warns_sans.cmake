function (target_enable_warnings target)
    target_compile_options(
        ${target}
        PUBLIC
            $<IF:$<CONFIG:Debug>,-Wall -Wpedantic -Wextra -Wshadow -Wno-sign-compare,>
    )
endfunction()

function (target_enable_sanitizers target)
    target_compile_options(
        ${target} 
        PUBLIC
            $<IF:$<CONFIG:Debug>,
            -fsanitize=signed-integer-overflow -fsanitize=pointer-overflow
            -fsanitize=shift -fsanitize=address -fsanitize=bounds
            -fsanitize=integer-divide-by-zero -fsanitize=float-divide-by-zero
            -fsanitize=return -fsanitize=null -fsanitize=undefined,>
    )
    target_link_options(
        ${target} 
        PUBLIC
            $<IF:$<CONFIG:Debug>,-fsanitize=address -fsanitize=undefined,>
    )
endfunction()