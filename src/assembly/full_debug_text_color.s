.global full_debug_text_color
.extern full_debug_text_color_branchback

full_debug_text_color:

lis r5, debug_text_color@h
ori r5, r5, debug_text_color@l
lwz r6, 0x0(r5)
cmpwi r6, 0
beq end
mr r4, r5

end:
li r3, 1 // Overwritten instruction
lis r5, full_debug_text_color_branchback@h
ori r5, r5, full_debug_text_color_branchback@l
mtctr r5
bctr
