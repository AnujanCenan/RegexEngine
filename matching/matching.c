#include "../parsing/include/parser.h"
#include <string.h>

#define REGEX_EXPRESSION 1
#define TEXT_STRING 2

typedef struct {
    char* text_string;
    int text_len;
    int text_ptr;
    bool match_found;
} TextInfo;

bool match_sub_expression(TextInfo* text, SubExpr* sub, SubExpr* sub_parent);
bool match_or_expression(TextInfo* text, OrExpr* or_expr, SubExpr* sub_parent);

bool match_expression(TextInfo* text, Expr* expr, SubExpr* sub_parent)
{
    if (text->match_found) return true;

    if (sub_parent && !sub_parent->repeated_block->star->is_empty) 
    {
        int curr_text_ptr = text->text_ptr;
        bool left_result = match_sub_expression(text, expr->sub, sub_parent);
        Expr* original_expr = (Expr *) sub_parent->repeated_block->block->data;
        if (text->match_found) return true;

        if (left_result && text->text_ptr > curr_text_ptr) 
        {
            // Note the curr_text_ptr points to the location of a match
            bool result = match_expression(text, original_expr, sub_parent);
            if (!result)
            {
                text->text_ptr = curr_text_ptr;
            } else
            {
                return true;
            }
        }
        
        bool right_result = match_or_expression(text, expr->or_expr, sub_parent);
        if (text->match_found) return true;

        if (right_result && text->text_ptr > curr_text_ptr)
        {
            bool result = match_expression(text, original_expr, sub_parent);
            if (!result)
            {
                text->text_ptr = curr_text_ptr;
            } else
            {
                return true;
            }
        }

        return match_sub_expression(text, sub_parent->sub_expr, NULL);      // Shouldnt say NULL; should be precise about what the 'sub parent' is. REmember that subparent means the parent of an EXPRESSION NODE specifically
    } else
    {
        // left child
        int curr_text_ptr = text->text_ptr;
        bool result = match_sub_expression(text, expr->sub, sub_parent);
        printf("Left child result = %d\n", result);
        if (result) return result;
        
        text->text_ptr = curr_text_ptr;
        if (!expr->or_expr->is_empty)
        {
            // right child
            result = match_or_expression(text, expr->or_expr, sub_parent);
        }

        printf("Right child result = %d\n", result);
        return result;
    }
}

bool match_sub_expression(TextInfo* text, SubExpr* sub, SubExpr* sub_parent)
{
    if (text->match_found) return true;

    if (sub->is_empty)
    {
        if (sub_parent && !sub_parent->repeated_block->star->is_empty)
        {
            // got through a complete match for a bracketed expression that 
            // has a star attached -- need to try match this bracketed expression
            // again with the rest of the string
            return true;
        } else if (sub_parent)
        {
            return match_sub_expression(text, sub_parent->sub_expr, NULL);     // inaccurate; sub_parent not necessarily null if grouping is nested
        } else
        {
            printf("\tHit the empty sub expression; text_ptr = %d;; text_len = %d; returning %d\n", text->text_ptr, text->text_len, text->text_ptr >= text->text_len);
            text->match_found = text->text_ptr >= text->text_len;
            return text->match_found;
        }
    } else if (sub->repeated_block->block->type == BT_SingleCharType && sub->repeated_block->star->is_empty)
    {
        if (text->text_ptr >= text->text_len) return false;
        char char_data = ((SingleChar *) sub->repeated_block->block->data)->char_data;      // need to handle escape chars

        if (char_data == '.'|| text->text_string[text->text_ptr] == char_data)      
        {
            printf("text_ptr = %d; char data = %c\n", text->text_ptr, char_data);

            text->text_ptr++;
            return match_sub_expression(text, sub->sub_expr, sub_parent);
        } else
        {
            return false;
        }
    } else if (sub->repeated_block->block->type == BT_SingleCharType && !sub->repeated_block->star->is_empty)       // change field is_empty for star to has_star
    {
        char char_data = ((SingleChar *) sub->repeated_block->block->data)->char_data;      // need to handle escape chars
        int curr_text_ptr = text->text_ptr;
        if ((char_data == '.' && text->text_ptr < text->text_len) || text->text_string[text->text_ptr] == char_data)      
        {
            text->text_ptr++;
            bool match_again = match_sub_expression(text, sub, sub_parent);
            if (match_again) return true;
        }

        text->text_ptr = curr_text_ptr;
        return match_sub_expression(text, sub->sub_expr, sub_parent);
    } else if (sub->repeated_block->block->type == BT_ExpressionType)
    {
        Expr* expr = (Expr *) sub->repeated_block->block->data;
        SubExpr* new_sub_parent = sub;

        /**
         * Prioritise a 'sub (grand) parent' that has a star over a 'younger' parent that does not have a star
         * We want to be able to ask for more star-match attempts
         */
        bool old_parent_has_star = (sub_parent && !sub_parent->repeated_block->star->is_empty);
        bool curr_sub_has_star = !sub->repeated_block->star->is_empty;
        if (old_parent_has_star && !curr_sub_has_star) new_sub_parent = sub_parent;

        return match_expression(text, expr, new_sub_parent);
    }

    return false;
}

/**
 * Assumes or_expr is NOT empty. Gives a false result if an empty or_expr is given
 */
bool match_or_expression(TextInfo* text, OrExpr* or_expr, SubExpr* sub_parent)
{
    if (text->match_found) return true;
    
    if (or_expr->is_empty) {
        return false;
    }

    return match_expression(text, or_expr->expr, sub_parent);
}



int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "USAGE: <program_name> <regex_expression> <text_string>\n");
        exit(1);
    }

    Expr* tree = parse_expression(&argv[REGEX_EXPRESSION]);
    print_tree(tree);
    printf("\n");
    int text_len = strlen(argv[TEXT_STRING]);
    TextInfo text = { .text_string=argv[TEXT_STRING], .text_len=text_len, .text_ptr=0, .match_found=false };

    


    match_expression(&text, tree, NULL) ? printf("MATCH FOUND!\n") : printf("NO MATCH FOUND!\n");

    free_expression(tree);

    return 0;
}