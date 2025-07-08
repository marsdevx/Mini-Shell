/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_checker.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 18:20:53 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:24:31 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	is_redirect(t_e_type t)
{
	return (t == REDIRECT_IN || t == REDIRECT_OUT || t == REDIRECT_APPEND
		|| t == HEREDOC);
}

int	is_text(t_e_type t)
{
	return (t == WORD || t == FIELD || t == EXP_FIELD);
}

int	is_valid_var_char(char c, int first)
{
	if (first)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}
